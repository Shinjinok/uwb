/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mixer.c
 *
 * Mixer utility.
 */

#include <px4_config.h>
#include <px4_posix.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include <systemlib/err.h>
#include <systemlib/mixer/mixer.h>
#include <uORB/topics/actuator_controls.h>

/**
 * Mixer utility for loading mixer files to devices
 *
 * @ingroup apps
 */
extern "C" __EXPORT int mixer_main(int argc, char *argv[]);

static void	usage(const char *reason);
static int	load(const char *devname, const char *fname);
static int  mixer_list(const char *devname);
static int  param_list(const char *devname, int mix_index);

int
mixer_main(int argc, char *argv[])
{
	if (argc < 2) {
		usage("missing command");
		return 1;
	}

    if (!strcmp(argv[1], "load")) {
        if (argc < 4) {
            usage("missing device or filename");
            return 1;
        }

        int ret = load(argv[2], argv[3]);

        if (ret != 0) {
            warnx("failed to load mixer");
            return 1;
        }

    } else 	if (!strcmp(argv[1], "list")) {
        if (argc < 3) {
            usage("missing device");
            return 1;
        }

        int ret = mixer_list(argv[2]);

        if (ret != 0) {
            warnx("failed to list mixers");
            return 1;
        }

    }  else if (!strcmp(argv[1], "params")) {
        if (argc < 4) {
            usage("missing device or mixer index");
            return 1;
        }

        int ret = param_list(argv[2], strtoul(argv[3], NULL, 0) );

        if (ret != 0) {
            warnx("failed to list parameters");
            return 1;
        }

    } else {
		usage("Unknown command");
		return 1;
	}

	return 0;
}

static void
usage(const char *reason)
{
	if (reason && *reason) {
		PX4_INFO("%s", reason);
	}

	PX4_INFO("usage:");
    PX4_INFO("  mixer load <device> <filename>");
    PX4_INFO("  mixer list <device>");
    PX4_INFO("  mixer params <device> <mixer_index>");
//    PX4_INFO("  mixer getparam <mixer_id> <param_id>");
//    PX4_INFO("  mixer setparam <mixer_id> <param_id> <value>");
}

static int
load(const char *devname, const char *fname)
{
	// sleep a while to ensure device has been set up
	usleep(20000);

	int dev;

	/* open the device */
	if ((dev = px4_open(devname, 0)) < 0) {
		warnx("can't open %s\n", devname);
		return 1;
	}

	/* reset mixers on the device */
	if (px4_ioctl(dev, MIXERIOCRESET, 0)) {
		warnx("can't reset mixers on %s", devname);
		return 1;
	}

	char buf[2048];

	if (load_mixer_file(fname, &buf[0], sizeof(buf)) < 0) {
		warnx("can't load mixer: %s", fname);
		return 1;
	}

	/* Pass the buffer to the device */
	int ret = px4_ioctl(dev, MIXERIOCLOADBUF, (unsigned long)buf);

	if (ret < 0) {
		warnx("error loading mixers from %s", fname);
		return 1;
	}

	return 0;
}


static int
mixer_list(const char *devname)
{
    int dev;

    /* open the device */
    if ((dev = px4_open(devname, 0)) < 0) {
        warnx("can't open %s\n", devname);
        return 1;
    }

    unsigned mix_count;

    mixer_id_e id;

    /* Get the mixer count */
    int ret = px4_ioctl(dev, MIXERIOCGETMIXERCOUNT, (unsigned long)&mix_count);

    if (ret < 0) {
        warnx("can't get mixer count for:%s", devname);
        return 1;
    }

    PX4_INFO("List of mixers:");

    printf("Mixer count : %u \n", mix_count);

    for(int index=0; index < mix_count; index++){
        printf("mixer index %u : ", index);
        id.index = index;
        /* Get the mixer name at index*/
        ret = px4_ioctl(dev, MIXERIONAME, (unsigned long)&id);
        printf(id.id);
        printf("\n");
    }

    if (ret < 0) {
        warnx("can't get mixer id for:%s", devname);
        return 1;
    }

    return 0;
}


static int
param_list(const char *devname, int mix_index)
{
    int dev;

    /* open the device */
    if ((dev = px4_open(devname, 0)) < 0) {
        warnx("can't open %s\n", devname);
        return 1;
    }

//    unsigned mix_count;
//    /* Get the mixer count */
//    int ret = px4_ioctl(dev, MIXERIOCGETMIXERCOUNT, (unsigned long)&mix_count);

//    if (ret < 0) {
//        warnx("can't get mixer count for:%s", devname);
//        return 1;
//    }

    mixer_param_id_s param_ids;
    param_ids.mix_index = mix_index;
    /* Get the mixer paramer identifiers*/
    int ret = px4_ioctl(dev, MIXERIOGETPARAMIDS, (unsigned long)&param_ids);

    if (ret < 0) {
        warnx("can't get mixer :%s parameters for mixer %u", devname, mix_index);
        return 1;
    }

    if (param_ids.id_count == 0) {
        printf("mixer:%u  parameter list empty\n", mix_index);
        return 0;
    }

    mixer_param_s param;
    for(int index=0; index < param_ids.id_count; index++){
        param.mix_index = mix_index;
        param.param_index = index;
        ret = px4_ioctl(dev, MIXERIOGETPARAM, (unsigned long)&param);
        printf("mixer:%u  param:%u id:%s value:%.2f\n", mix_index, index, param_ids.ids[index], (double) param.value);
    }

    if (ret < 0) {
        warnx("can't get mixer id for:%s", devname);
        return 1;
    }

    return 0;
}
