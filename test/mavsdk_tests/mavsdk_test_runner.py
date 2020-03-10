#!/usr/bin/env python3

import argparse
import atexit
import datetime
import errno
import json
import os
import psutil
import subprocess
import sys
import signal


class Runner:
    def __init__(self, log_dir, verbose):
        self.cmd = ""
        self.cwd = None
        self.args = []
        self.env = {}
        self.log_prefix = ""
        self.log_dir = log_dir
        self.log_filename = ""
        self.log_fd = None
        self.verbose = verbose

    def create_log_filename(self, model, test_filter):
        return self.log_dir + os.path.sep + \
            "log-{}-{}-{}-{}.log".format(
                self.log_prefix,
                model,
                test_filter,
                datetime.datetime.now().strftime("%Y-%m-%dT%H-%M-%SZ"))

    def start(self, config):
        if self.verbose:
            print("Running: {}".format(" ".join([self.cmd] + self.args)))

        atexit.register(self.stop)

        if self.verbose:
            self.log_fd = None
        else:
            self.log_filename = self.create_log_filename(
                config['model'], config['test_filter'])
            self.log_fd = open(self.log_filename, 'w')

        self.process = subprocess.Popen(
            [self.cmd] + self.args,
            cwd=self.cwd,
            env=self.env,
            stdout=self.log_fd,
            stderr=self.log_fd
        )

    def wait(self, timeout_min):
        try:
            return self.process.wait(timeout=timeout_min*60)
        except subprocess.TimeoutExpired:
            print("Timeout of {} min{} reached, stopping...".
                  format(timeout_min, "s" if timeout_min > 1 else ""))
            self.stop()
            print("stopped.")
            return errno.ETIMEDOUT

    def stop(self):
        atexit.unregister(self.stop)

        self.log_fd.flush()

        returncode = self.process.poll()
        if returncode is not None:
            self.log_fd.close()
            return returncode

        if self.verbose:
            print("Sending SIGINT to {}".format(self.process.pid))
        self.process.send_signal(signal.SIGINT)
        try:
            self.log_fd.close()
            return self.process.wait(timeout=1)
        except subprocess.TimeoutExpired:
            pass

        if self.verbose:
            print("Terminating {}".format(self.process.pid))
        self.process.terminate()

        try:
            self.log_fd.close()
            return self.process.wait(timeout=1)
        except subprocess.TimeoutExpired:
            pass

        if self.verbose:
            print("Killing {}".format(self.process.pid))
        self.process.kill()

        if self.verbose:
            print("{} exited with {}".format(
                self.command, self.process.returncode))

        self.log_fd.close()
        return self.process.returncode

    def print_output(self):
        with open(self.log_filename, 'r') as f:
            print(f.read(), end="")


class Px4Runner(Runner):
    def __init__(self, model, workspace_dir, log_dir, speed_factor, debugger,
                 verbose):
        super().__init__(log_dir, verbose)
        self.cmd = workspace_dir + "/build/px4_sitl_default/bin/px4"
        self.cwd = workspace_dir + "/build/px4_sitl_default/tmp/rootfs"
        self.args = [
                workspace_dir + "/ROMFS/px4fmu_common",
                "-s",
                "etc/init.d-posix/rcS",
                "-t",
                workspace_dir + "/test_data",
                "-d"
            ]
        self.env = {"PATH": os.environ['PATH'],
                    "PX4_SIM_MODEL": model,
                    "PX4_SIM_SPEED_FACTOR": str(speed_factor)}
        self.log_prefix = "px4"

        if not debugger:
            pass
        elif debugger == "valgrind":
            self.args = ["--track-origins=yes", "--leak-check=full", "-v",
                         self.cmd] + self.args
            self.cmd = "valgrind"
        elif debugger == "callgrind":
            self.args = ["--tool=callgrind", "-v", self.cmd] + self.args
            self.cmd = "valgrind"
        elif debugger == "gdb":
            self.args = ["--args", self.cmd] + self.args
            self.cmd = "gdb"
        else:
            print("Using custom debugger ", debugger)
            self.args = [self.cmd] + self.args
            self.cmd = debugger


class GzserverRunner(Runner):
    def __init__(self, model, workspace_dir, log_dir, speed_factor, verbose):
        super().__init__(log_dir, verbose)
        self.env = {"PATH": os.environ['PATH'],
                    "HOME": os.environ['HOME'],
                    "GAZEBO_PLUGIN_PATH":
                    workspace_dir + "/build/px4_sitl_default/build_gazebo",
                    "GAZEBO_MODEL_PATH":
                    workspace_dir + "/Tools/sitl_gazebo/models",
                    "PX4_SIM_SPEED_FACTOR": str(speed_factor),
                    "DISPLAY": os.environ['DISPLAY']}
        self.cmd = "gzserver"
        self.args = ["--verbose",
                     workspace_dir + "/Tools/sitl_gazebo/worlds/" +
                     model + ".world"]
        self.log_prefix = "gzserver"


class GzclientRunner(Runner):
    def __init__(self, workspace_dir, log_dir, verbose):
        super().__init__(log_dir, verbose)
        self.env = {"PATH": os.environ['PATH'],
                    "HOME": os.environ['HOME'],
                    # "GAZEBO_PLUGIN_PATH":
                    # workspace_dir + "/build/px4_sitl_default/build_gazebo",
                    "GAZEBO_MODEL_PATH":
                    workspace_dir + "/Tools/sitl_gazebo/models",
                    "DISPLAY": os.environ['DISPLAY']}
        self.cmd = "gzclient"
        self.args = ["--verbose"]
        self.log_prefix = "gzclient"


class TestRunner(Runner):
    def __init__(self, workspace_dir, log_dir, config, test,
                 mavlink_connection, verbose):
        super().__init__(log_dir, verbose)
        self.env = {"PATH": os.environ['PATH']}
        self.cmd = workspace_dir + \
            "/build/px4_sitl_default/mavsdk_tests/mavsdk_tests"
        self.args = ["--url", mavlink_connection, test]
        self.log_prefix = "test_runner"


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("--log-dir",
                        help="Directory for log files", default="logs")
    parser.add_argument("--speed-factor", default=1,
                        help="How fast to run the simulation")
    parser.add_argument("--iterations", type=int, default=1,
                        help="How often to run all tests")
    parser.add_argument("--abort-early", action='store_true',
                        help="Abort on first unsuccessful test")
    parser.add_argument("--gui", default=False, action='store_true',
                        help="Display gzclient with simulation")
    parser.add_argument("--model", type=str, default='all',
                        help="Specify which model to run")
    parser.add_argument("--debugger", default="",
                        help="valgrind callgrind gdb lldb")
    parser.add_argument("--verbose", default=False, action='store_true',
                        help="Enable more verbose output")
    parser.add_argument("config_file", help="JSON config file to use")
    args = parser.parse_args()

    with open(args.config_file) as json_file:
        config = json.load(json_file)

    if config["mode"] != "sitl" and args.gui:
        print("--gui is not compatible with the mode '{}'"
              .format(config["mode"]))
        sys.exit(1)

    if not is_everything_ready(config):
        sys.exit(1)

    if args.verbose:
        print("Creating directory: {}".format(args.log_dir))
    os.makedirs(args.log_dir, exist_ok=True)

    run(args, config)


def determine_tests(workspace_dir, filter):
    cmd = workspace_dir + "/build/px4_sitl_default/mavsdk_tests/mavsdk_tests"
    args = ["--list-test-names-only", filter]
    p = subprocess.Popen(
        [cmd] + args,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT)
    tests = str(p.stdout.read().decode("utf-8")).strip().split('\n')
    return tests


def is_running(process_name):
    for proc in psutil.process_iter(attrs=['name']):
        if proc.info['name'] == process_name:
            return True
    return False


def is_everything_ready(config):
    result = True

    if config['mode'] == 'sitl':
        if is_running('px4'):
            print("px4 process already running\n"
                  "run `killall px4` and try again")
            result = False
        if not os.path.isfile('build/px4_sitl_default/bin/px4'):
            print("PX4 SITL is not built\n"
                  "run `DONT_RUN=1 "
                  "make px4_sitl gazebo mavsdk_tests`")
            result = False
        if config['simulator'] == 'gazebo':
            if is_running('gzserver'):
                print("gzserver process already running\n"
                      "run `killall gzserver` and try again")
                result = False
            if is_running('gzclient'):
                print("gzclient process already running\n"
                      "run `killall gzclient` and try again")
                result = False

    if not os.path.isfile('build/px4_sitl_default/mavsdk_tests/mavsdk_tests'):
        print("Test runner is not built\n"
              "run `DONT_RUN=1 "
              "make px4_sitl gazebo mavsdk_tests`")
        result = False

    return result


def run(args, config):
    overall_success = True

    for iteration in range(args.iterations):
        if args.iterations > 1:
            print("Test iteration: {}".format(iteration + 1, args.iterations))

        was_success = run_test_group(args, config)

        if not was_success:
            overall_success = False

        if args.iterations > 1 and not was_success and args.abort_early:
            print("Aborting with a failure in test run {}/{}".
                  format(iteration + 1, args.iterations))
            break

    print("Overall result: {}".
          format("SUCCESS" if overall_success else "FAIL"))
    sys.exit(0 if overall_success else 1)


def run_test_group(args, config):
    overall_success = True

    test_matrix = config["tests"]

    if args.model == 'all':
        models = test_matrix
    else:
        found = False
        for elem in test_matrix:
            if elem['model'] == args.model:
                models = [elem]
                found = True
        if not found:
            print("Specified model is not defined")
            models = []

    for group in models:
        print("Running test group for '{}' with filter '{}'"
              .format(group['model'], group['test_filter']))

        tests = determine_tests(os.getcwd(), group['test_filter'])

        for test in tests:
            print("Running test '{}'".format(test))
            was_success = run_test(test, group, args, config)
            print("Test '{}': {}".
                  format(test, "Success" if was_success else "Fail"))

            if not was_success:
                overall_success = False

            if not was_success and args.abort_early:
                print("Aborting early")
                return False

    return overall_success


def run_test(test, group, args, config):

    speed_factor = args.speed_factor
    if "max_speed_factor" in group:
        speed_factor = min(int(speed_factor), group["max_speed_factor"])

    if config['mode'] == 'sitl':
        px4_runner = Px4Runner(
            group['model'], os.getcwd(), args.log_dir, speed_factor,
            args.debugger, args.verbose)
        px4_runner.start(group)

        if config['simulator'] == 'gazebo':
            gzserver_runner = GzserverRunner(
                group['model'], os.getcwd(), args.log_dir, speed_factor,
                args.verbose)
            gzserver_runner.start(group)

            if args.gui:
                gzclient_runner = GzclientRunner(
                    os.getcwd(), args.log_dir, args.verbose)
                gzclient_runner.start(group)

    test_runner = TestRunner(os.getcwd(), args.log_dir, group, test,
                             config['mavlink_connection'], args.verbose)
    test_runner.start(group)

    returncode = test_runner.wait(group['timeout_min'])
    is_success = (returncode == 0)

    if config['mode'] == 'sitl':
        if config['simulator'] == 'gazebo':
            if args.gui:
                gzclient_runner.stop()
            gzserver_runner.stop()
        px4_runner.stop()

    if not is_success and not args.verbose:
        test_runner.print_output()

    return is_success


if __name__ == '__main__':
    main()
