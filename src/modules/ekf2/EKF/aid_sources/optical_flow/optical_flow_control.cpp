/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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
 * @file optical_flow_control.cpp
 * Control functions for optical flow fusion
 */

#include "ekf.h"

void Ekf::controlOpticalFlowFusion(const imuSample &imu_delayed)
{
	if (!_flow_buffer || (_params.flow_ctrl != 1)) {
		stopFlowFusion();
		return;
	}

	bool flow_data_ready = false;

	// New optical flow data is available and is ready to be fused when the midpoint of the sample falls behind the fusion time horizon
	if (_flow_buffer->pop_first_older_than(imu_delayed.time_us, &_flow_sample_delayed)) {

		// flow gyro has opposite sign convention
		_ref_body_rate = -(imu_delayed.delta_ang / imu_delayed.delta_ang_dt - getGyroBias());

		// ensure valid flow sample gyro rate before proceeding
		if (!PX4_ISFINITE(_flow_sample_delayed.gyro_rate(0)) || !PX4_ISFINITE(_flow_sample_delayed.gyro_rate(1))) {
			_flow_sample_delayed.gyro_rate = _ref_body_rate;

		} else if (!PX4_ISFINITE(_flow_sample_delayed.gyro_rate(2))) {
			// Some flow modules only provide X ind Y angular rates. If this is the case, complete the vector with our own Z gyro
			_flow_sample_delayed.gyro_rate(2) = _ref_body_rate(2);
		}

		const flowSample &flow_sample = _flow_sample_delayed;

		const int32_t min_quality = _control_status.flags.in_air
					    ? _params.flow_qual_min
					    : _params.flow_qual_min_gnd;

		const bool is_quality_good = (flow_sample.quality >= min_quality);
		const bool is_magnitude_good = flow_sample.flow_rate.isAllFinite()
					       && !flow_sample.flow_rate.longerThan(_flow_max_rate);

		bool is_tilt_good = true;

#if defined(CONFIG_EKF2_RANGE_FINDER)
		is_tilt_good = (_R_to_earth(2, 2) > _params.range_cos_max_tilt);

#endif // CONFIG_EKF2_RANGE_FINDER

		calcOptFlowBodyRateComp(flow_sample);

		if (is_quality_good
		    && is_magnitude_good
		    && is_tilt_good) {

			flow_data_ready = true;
		}

		updateOptFlow(_aid_src_optical_flow, flow_sample);

		// logging
		const Vector3f flow_gyro_corrected = flow_sample.gyro_rate - _flow_gyro_bias;
		_flow_rate_compensated = flow_sample.flow_rate - flow_gyro_corrected.xy();
	}

	if (flow_data_ready) {

		// Check if we are in-air and require optical flow to control position drift
		bool is_flow_required = _control_status.flags.in_air
					&& (_control_status.flags.inertial_dead_reckoning // is doing inertial dead-reckoning so must constrain drift urgently
					    || isOnlyActiveSourceOfHorizontalAiding(_control_status.flags.opt_flow));

		const bool is_within_max_sensor_dist = getHagl() <= _flow_max_distance;

		const bool continuing_conditions_passing = (_params.flow_ctrl == 1)
				&& _control_status.flags.tilt_align
				&& is_within_max_sensor_dist;

		const bool starting_conditions_passing = continuing_conditions_passing
				&& isTimedOut(_aid_src_optical_flow.time_last_fuse, (uint64_t)2e6); // Prevent rapid switching

		// If the height is relative to the ground, terrain height cannot be observed.
		_hagl_sensor_status.flags.flow = _control_status.flags.opt_flow && !(_height_sensor_ref == HeightSensor::RANGE);

		if (_control_status.flags.opt_flow) {
			if (continuing_conditions_passing) {
				fuseOptFlow(_hagl_sensor_status.flags.flow);

				// handle the case when we have optical flow, are reliant on it, but have not been using it for an extended period
				if (isTimedOut(_aid_src_optical_flow.time_last_fuse, _params.no_aid_timeout_max)) {
					if (is_flow_required) {
						resetFlowFusion();

					} else {
						stopFlowFusion();
					}
				}

			} else {
				stopFlowFusion();
			}

		} else {
			if (starting_conditions_passing) {
				startFlowFusion();
			}
		}

	} else if (_control_status.flags.opt_flow && isTimedOut(_flow_sample_delayed.time_us, _params.reset_timeout_max)) {
		stopFlowFusion();
	}
}

void Ekf::startFlowFusion()
{
	if (_height_sensor_ref != HeightSensor::RANGE) {
		// If the height is relative to the ground, terrain height cannot be observed.
		_hagl_sensor_status.flags.flow = true;
	}

	if (isHorizontalAidingActive()) {
		if (fuseOptFlow(_hagl_sensor_status.flags.flow)) {
			ECL_INFO("starting optical flow");
			_control_status.flags.opt_flow = true;

		} else if (_hagl_sensor_status.flags.flow && !_hagl_sensor_status.flags.range_finder) {
			ECL_INFO("starting optical flow, resetting terrain");
			resetTerrainToFlow();
			_control_status.flags.opt_flow = true;
		}

	} else {
		if (isTerrainEstimateValid() || (_height_sensor_ref == HeightSensor::RANGE)) {
			ECL_INFO("starting optical flow, resetting");
			resetFlowFusion();
			_control_status.flags.opt_flow = true;

		} else if (_hagl_sensor_status.flags.flow) {
			ECL_INFO("starting optical flow, resetting terrain");
			resetTerrainToFlow();
			_control_status.flags.opt_flow = true;
		}
	}

	_hagl_sensor_status.flags.flow = _control_status.flags.opt_flow && !(_height_sensor_ref == HeightSensor::RANGE);
}

void Ekf::resetFlowFusion()
{
	ECL_INFO("reset velocity to flow");
	_information_events.flags.reset_vel_to_flow = true;
	resetHorizontalVelocityTo(_flow_vel_ne, calcOptFlowMeasVar(_flow_sample_delayed));

	// reset position, estimate is relative to initial position in this mode, so we start with zero error
	if (!_control_status.flags.in_air) {
		ECL_INFO("reset position to zero");
		resetHorizontalPositionTo(Vector2f(0.f, 0.f), 0.f);
	}

	resetAidSourceStatusZeroInnovation(_aid_src_optical_flow);

	_innov_check_fail_status.flags.reject_optflow_X = false;
	_innov_check_fail_status.flags.reject_optflow_Y = false;
}

void Ekf::resetTerrainToFlow()
{
	ECL_INFO("reset hagl to flow");
	// TODO: use the flow data
	_state.terrain = fmaxf(0.0f, _state.pos(2));
	P.uncorrelateCovarianceSetVariance<State::terrain.dof>(State::terrain.idx, 100.f);
	_terrain_vpos_reset_counter++;

	resetAidSourceStatusZeroInnovation(_aid_src_optical_flow);

	_innov_check_fail_status.flags.reject_optflow_X = false;
	_innov_check_fail_status.flags.reject_optflow_Y = false;
}

void Ekf::stopFlowFusion()
{
	if (_control_status.flags.opt_flow) {
		ECL_INFO("stopping optical flow fusion");
		_control_status.flags.opt_flow = false;
		_hagl_sensor_status.flags.flow = false;
	}
}

void Ekf::calcOptFlowBodyRateComp(const flowSample &flow_sample)
{
	// calculate the bias estimate using a combined LPF and spike filter
	_flow_gyro_bias = 0.99f * _flow_gyro_bias
			  + 0.01f * matrix::constrain(flow_sample.gyro_rate - _ref_body_rate, -0.1f, 0.1f);
}
