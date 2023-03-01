/****************************************************************************
 *
 *   Copyright (c) 2020 PX4 Development Team. All rights reserved.
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
 *
 * This module is a modification of the hippocampus control module and is designed for the
 * BlueROV2.
 *
 * All the acknowledgments and credits for the fw wing app are reported in those files.
 *
 * @author Tim Hansen <t.hansen@jacobs-university.de>
 * @author Daniel Duecker <daniel.duecker@tuhh.de>
 */

#include "usv_omni_control.hpp"


/**
 * USV pos_controller app start / stop handling function
 *
 * @ingroup apps
 */
extern "C" __EXPORT int usv_omni_control_main(int argc, char *argv[]);


USVOmniControl::USVOmniControl():
	ModuleParams(nullptr),
	WorkItem(MODULE_NAME, px4::wq_configurations::nav_and_controllers),
	/* performance counters */
	_loop_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": cycle"))
{
}

USVOmniControl::~USVOmniControl()
{
	perf_free(_loop_perf);
}

bool USVOmniControl::init()
{
	if (!_vehicle_local_position_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		return false;
	}

	return true;
}

void USVOmniControl::parameters_update(bool force)
{
	// check for parameter updates
	if (_parameter_update_sub.updated() || force) {
		// clear update
		parameter_update_s pupdate;
		_parameter_update_sub.copy(&pupdate);

		// update parameters from storage
		updateParams();
	}
}

void USVOmniControl::publishAttitudeSetpoint(const float thrust_x, const float thrust_y, const float thrust_z,
		const float roll_des, const float pitch_des, const float yaw_des)
{
	//watch if inputs are not to high
	vehicle_attitude_setpoint_s vehicle_attitude_setpoint = {};
	vehicle_attitude_setpoint.timestamp = hrt_absolute_time();

	vehicle_attitude_setpoint.roll_body = roll_des;
	vehicle_attitude_setpoint.pitch_body = pitch_des;
	vehicle_attitude_setpoint.yaw_body = yaw_des;

	vehicle_attitude_setpoint.thrust_body[0] = thrust_x;
	vehicle_attitude_setpoint.thrust_body[1] = thrust_y;
	vehicle_attitude_setpoint.thrust_body[2] = thrust_z;


	_att_sp_pub.publish(vehicle_attitude_setpoint);
}

void USVOmniControl::pose_controller_6dof(const Vector3f &pos_des,
		const float roll_des, const float pitch_des, const float yaw_des,
		vehicle_attitude_s &vehicle_attitude, vehicle_local_position_s &vlocal_pos)
{
	//get current rotation of vehicle
	Quatf q_att(vehicle_attitude.q);

	Vector3f p_control_output = Vector3f(_param_pose_gain_x.get() * (pos_des(0) - vlocal_pos.x) - _param_pose_gain_d_x.get() * vlocal_pos.vx,
					     _param_pose_gain_y.get() * (pos_des(1) - vlocal_pos.y) - _param_pose_gain_d_y.get() * vlocal_pos.vy,
					     _param_pose_gain_z.get() * (pos_des(2) - vlocal_pos.z) - _param_pose_gain_d_z.get() * vlocal_pos.vz);

	Vector3f rotated_input = q_att.rotateVectorInverse(p_control_output);//rotate the coord.sys (from global to body)

	publishAttitudeSetpoint(rotated_input(0),
				  rotated_input(1),
				  rotated_input(2),
				  roll_des, pitch_des, yaw_des);

}

void USVOmniControl::stabilization_controller_6dof(const Vector3f &pos_des,
		const float roll_des, const float pitch_des, const float yaw_des,
		vehicle_attitude_s &vehicle_attitude, vehicle_local_position_s &vlocal_pos)
{
	//get current rotation of vehicle
	Quatf q_att(vehicle_attitude.q);

	Vector3f p_control_output = Vector3f(0,
					     0,
					     _param_pose_gain_z.get() * (pos_des(2) - vlocal_pos.z));
	//potential d controller missing
	Vector3f rotated_input = q_att.rotateVectorInverse(p_control_output);//rotate the coord.sys (from global to body)

	publishAttitudeSetpoint(rotated_input(0) + pos_des(0), rotated_input(1) + pos_des(1), rotated_input(2),
				  roll_des, pitch_des, yaw_des);

}


void USVOmniControl::handleManualInputs(const manual_control_setpoint_s &manual_control_setpoint)
{
	// TODO: POSITION MODE, MANUAL MODE
	// handle all the different modes that use manual_control_setpoint
	if (_vehicle_control_mode.flag_control_manual_enabled && !_vehicle_control_mode.flag_control_rates_enabled) {
		/* manual/direct control */
		_thrust_setpoint.setAll(0.0f);
		_thrust_setpoint(0) = manual_control_setpoint.throttle;
		_thrust_setpoint(1) = manual_control_setpoint.roll;

		_torque_setpoint.setAll(0.0f);
		_torque_setpoint(0) = manual_control_setpoint.yaw;
	}
}

void USVOmniControl::handleVelocityInputs()
{
}

void USVOmniControl::handlePositionInputs()
{
}



bool USVOmniControl::controlPosition(const matrix::Vector2d & global_pos, const matrix::Vector3f & ground_speed, const position_setpoint_triplet_s & _pos_sp_triplet)
{
return false;
}

void USVOmniControl::controlVelocity(const matrix::Vector3f & current_velocity)
{
}

void USVOmniControl::controlAttitude(
	const vehicle_attitude_s &attitude,
	const vehicle_attitude_setpoint_s &attitude_setpoint,
	const vehicle_angular_velocity_s &angular_velocity,
	const vehicle_rates_setpoint_s &rates_setpoint)
{
	/* Geometric Controller from UUV*/
	// IT KINDA WORKS, I GUESS
	Eulerf euler_angles(matrix::Quatf(attitude.q));

	float yaw_u;
	// pass through thrust
	float thrust_x;
	float thrust_y;
	float thrust_z;

	float yaw_body = attitude_setpoint.yaw_body;
	float yaw_rate_desired = rates_setpoint.yaw;

	/* get attitude setpoint rotational matrix */
	// TODO: simplify
	Dcmf rot_des = Eulerf(0.0f, 0.0f, yaw_body);

	/* get current rotation matrix from control state quaternions */
	Quatf q_att(attitude.q);
	Matrix3f rot_att =  matrix::Dcm<float>(q_att);

	Vector3f e_R_vec;
	Vector3f torques;

	/* Compute matrix: attitude error */
	Matrix3f e_R = (rot_des.transpose() * rot_att - rot_att.transpose() * rot_des) * 0.5;

	/* vee-map the error to get a vector instead of matrix e_R */
	e_R_vec(0) = e_R(2, 1);  /**< Roll  */
	e_R_vec(1) = e_R(0, 2);  /**< Pitch */
	e_R_vec(2) = e_R(1, 0);  /**< Yaw   */

	Vector3f omega{angular_velocity.xyz};
	omega(2) -= yaw_rate_desired;

	torques(2) = - e_R_vec(2) * _param_yaw_p.get(); /**< P-Control */
	torques(2) = torques(2) - omega(2) * _param_yaw_d.get(); /**< PD-Control */
	yaw_u = torques(2);

	// take thrust as is
	_thrust_setpoint.setAll(0.0f);
	_thrust_setpoint(0) = attitude_setpoint.thrust_body[0];
	_thrust_setpoint(1) = attitude_setpoint.thrust_body[1];

	_torque_setpoint.setAll(0.0f);
	_torque_setpoint(0) = yaw_u;
}

void USVOmniControl::publishTorqueSetpoint(const Vector3f &torque_sp, const hrt_abstime &timestamp_sample)
{
	float scaling = _param_max_torque_ac.get();
	vehicle_torque_setpoint_s result{};
	result.timestamp_sample = timestamp_sample;
	result.xyz[0] = 0.0f;
	result.xyz[1] = 0.0f;
	result.xyz[2] = (PX4_ISFINITE(torque_sp(2))) ? torque_sp(2) * scaling : 0.0f;
	result.timestamp = hrt_absolute_time();
	_vehicle_torque_setpoint_pub.publish(result);
}

void USVOmniControl::publishThrustSetpoint(const hrt_abstime &timestamp_sample)
{
	float scaling = _param_max_thrust_ac.get();
	vehicle_thrust_setpoint_s result{};
	result.timestamp_sample = timestamp_sample;
	_thrust_setpoint.copyTo(result.xyz);
	result.xyz[0] = (PX4_ISFINITE(_thrust_setpoint(0))) ? _thrust_setpoint(0) * scaling : 0.0f;
	result.xyz[1] = (PX4_ISFINITE(_thrust_setpoint(1))) ? _thrust_setpoint(1) * scaling : 0.0f;
	result.xyz[2] = 0.0f;  // Ignore X axis, because we are never flying
	result.timestamp = hrt_absolute_time();
	_vehicle_thrust_setpoint_pub.publish(result);
}

void USVOmniControl::Run()
{
	if (should_exit()) {
		_vehicle_local_position_sub.unregisterCallback();
		exit_and_cleanup();
		return;
	}

	perf_begin(_loop_perf);

	/* check vehicle control mode for changes to publication state */
	_vcontrol_mode_sub.update(&_control_mode);


	/* update parameters from storage */
	parameters_update();

	//vehicle_attitude_s attitude;
	vehicle_local_position_s vlocal_pos;

	/* only run controller if local_pos changed */
	if (_vehicle_local_position_sub.update(&vlocal_pos)) {

		/* Run geometric attitude controllers if NOT manual mode*/
		if (!_control_mode.flag_control_manual_enabled
		    && _control_mode.flag_control_attitude_enabled
		    && _control_mode.flag_control_rates_enabled) {

			_vehicle_attitude_sub.update(&_vehicle_attitude);//get current vehicle attitude
			_trajectory_setpoint_sub.update(&_trajectory_setpoint);

			float roll_des = 0;
			float pitch_des = 0;
			float yaw_des = _trajectory_setpoint.yaw;

			//stabilization controller(keep pos and hold depth + angle) vs position controller(global + yaw)
			if (_param_stabilization.get() == 0) {
				pose_controller_6dof(Vector3f(_trajectory_setpoint.position),
						     roll_des, pitch_des, yaw_des, _vehicle_attitude, vlocal_pos);

			} else {
				stabilization_controller_6dof(Vector3f(_trajectory_setpoint.position),
							      roll_des, pitch_des, yaw_des, _vehicle_attitude, vlocal_pos);
			}
		}
	}

	/* Manual Control mode (e.g. gamepad,...) - raw feedthrough no assistance */
	if (_manual_control_setpoint_sub.update(&_manual_control_setpoint)) {
		// This should be copied even if not in manual mode. Otherwise, the poll(...) call will keep
		// returning immediately and this loop will eat up resources.
		handleManualInputs(_manual_control_setpoint);
	}

			// Respond to an attitude update and run the attitude controller if enabled
	if (_control_mode.flag_control_attitude_enabled
		&& !_control_mode.flag_control_position_enabled
		&& !_control_mode.flag_control_velocity_enabled) {
		controlAttitude(_att, _att_sp);
	}

	/* Only publish if any of the proper modes are enabled */
	if (_control_mode.flag_control_manual_enabled ||
	    _control_mode.flag_control_attitude_enabled) {
		hrt_abstime timestamp = hrt_absolute_time();
		publishTorqueSetpoint(_torque_setpoint, timestamp);
		publishThrustSetpoint(timestamp);

	}

	perf_end(_loop_perf);
}

int USVOmniControl::task_spawn(int argc, char *argv[])
{
	USVOmniControl *instance = new USVOmniControl();

	if (instance) {
		_object.store(instance);
		_task_id = task_id_is_work_queue;

		if (instance->init()) {
			return PX4_OK;
		}

	} else {
		PX4_ERR("alloc failed");
	}

	delete instance;
	_object.store(nullptr);
	_task_id = -1;

	return PX4_ERROR;
}

int USVOmniControl::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}


int USVOmniControl::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Controls the attitude of an unmanned underwater vehicle (USV).
Publishes `actuator_controls_0` messages at a constant 250Hz.
### Implementation
Currently, this implementation supports only a few modes:
 * Full manual: Roll, pitch, yaw, and throttle controls are passed directly through to the actuators
 * Auto mission: The usv runs missions
### Examples
CLI usage example:
$ usv_omni_control start
$ usv_omni_control status
$ usv_omni_control stop
)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("usv_omni_control", "controller");
    PRINT_MODULE_USAGE_COMMAND("start")
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

int usv_omni_control_main(int argc, char *argv[])
{
    return USVOmniControl::main(argc, argv);
}
