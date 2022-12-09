/****************************************************************************
 *
 *   Copyright (c) 2021-2022 PX4 Development Team. All rights reserved.
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
 * @file gps_control.cpp
 * Control functions for ekf GNSS fusion
 */

#include "ekf.h"
#include <mathlib/mathlib.h>

void Ekf::controlGpsFusion()
{
	if (!((_params.gnss_ctrl & GnssCtrl::HPOS) || (_params.gnss_ctrl & GnssCtrl::VEL))) {
		stopGpsFusion();
		return;
	}

	// Check for new GPS data that has fallen behind the fusion time horizon
	if (_gps_data_ready) {

		const gpsSample &gps_sample{_gps_sample_delayed};

		const bool gps_checks_passing = isTimedOut(_last_gps_fail_us, (uint64_t)5e6);
		const bool gps_checks_failing = isTimedOut(_last_gps_pass_us, (uint64_t)5e6);

		controlGpsYawFusion(gps_sample, gps_checks_passing, gps_checks_failing);

		// GNSS velocity
		const float vel_var = sq(math::max(gps_sample.sacc, _params.gps_vel_noise));
		const Vector3f vel_obs_var(vel_var, vel_var, vel_var * sq(1.5f));
		updateVelocityAidSrcStatus(gps_sample.time_us,
					   gps_sample.vel,                                             // observation
					   vel_obs_var,                                                // observation variance
					   math::max(_params.gps_vel_innov_gate, 1.f),                 // innovation gate
					   _aid_src_gnss_vel);
		_aid_src_gnss_vel.fusion_enabled = (_params.gnss_ctrl & GnssCtrl::VEL);

		// GNSS position
		// relax the upper observation noise limit which prevents bad GPS perturbing the position estimate
		float pos_noise = math::max(gps_sample.hacc, _params.gps_pos_noise);

		if (!isOtherSourceOfHorizontalAidingThan(_control_status.flags.gps)) {
			// if we are not using another source of aiding, then we are reliant on the GPS
			// observations to constrain attitude errors and must limit the observation noise value.
			if (pos_noise > _params.pos_noaid_noise) {
				pos_noise = _params.pos_noaid_noise;
			}
		}

		const float pos_var = sq(pos_noise);
		const Vector2f pos_obs_var(pos_var, pos_var);
		updateHorizontalPositionAidSrcStatus(gps_sample.time_us,
						     gps_sample.pos,                             // observation
						     pos_obs_var,                                // observation variance
						     math::max(_params.gps_pos_innov_gate, 1.f), // innovation gate
						     _aid_src_gnss_pos);
		_aid_src_gnss_pos.fusion_enabled = (_params.gnss_ctrl & GnssCtrl::HPOS);

		// update GSF yaw estimator velocity (basic sanity check on GNSS velocity data)
		if (gps_checks_passing && !gps_checks_failing) {
			_yawEstimator.setVelocity(gps_sample.vel.xy(), gps_sample.sacc);
		}

		// Determine if we should use GPS aiding for velocity and horizontal position
		// To start using GPS we need angular alignment completed, the local NED origin set and GPS data that has not failed checks recently
		const bool mandatory_conditions_passing = ((_params.gnss_ctrl & GnssCtrl::HPOS) || (_params.gnss_ctrl & GnssCtrl::VEL))
				&& _control_status.flags.tilt_align
				&& _control_status.flags.yaw_align
				&& _NED_origin_initialised;

		const bool continuing_conditions_passing = mandatory_conditions_passing && !gps_checks_failing;
		const bool starting_conditions_passing = continuing_conditions_passing && gps_checks_passing;

		if (_control_status.flags.gps) {
			if (mandatory_conditions_passing) {
				if (continuing_conditions_passing
				    || !isOtherSourceOfHorizontalAidingThan(_control_status.flags.gps)) {

					fuseVelocity(_aid_src_gnss_vel);
					fuseHorizontalPosition(_aid_src_gnss_pos);

					bool do_vel_pos_reset = shouldResetGpsFusion();

					if (isYawFailure()
					    && _control_status.flags.in_air
					    && isTimedOut(_time_last_hor_vel_fuse, _params.EKFGSF_reset_delay)
					    && (_time_last_hor_vel_fuse > _time_last_on_ground_us)) {
						/* A rapid reset to the yaw emergency estimate is performed if horizontal velocity innovation checks continuously
						 * fails while the difference between the yaw emergency estimator and the yas estimate is large.
						 * This enables recovery from a bad yaw estimate. A reset is not performed if the fault condition was
						 * present before flight to prevent triggering due to GPS glitches or other sensor errors.
						 */
						if (resetYawToEKFGSF()) {
							ECL_WARN("GPS emergency yaw reset");
							do_vel_pos_reset = true;
						}
					}

					if (do_vel_pos_reset) {
						ECL_WARN("GPS fusion timeout, resetting velocity and position");
						_information_events.flags.reset_vel_to_gps = true;
						_information_events.flags.reset_pos_to_gps = true;
						resetVelocityTo(gps_sample.vel, vel_obs_var);
						resetHorizontalPositionTo(gps_sample.pos, pos_obs_var);

					} else if ((_imu_accel_clipping_NE || _imu_accel_clipping_D) && gps_checks_passing
						   && isTimedOut(_last_gnss_reset_accel_clipping_us, (uint64_t)3e6)) {
						// check for accel clipping and perform an emergency reset if GNSS is good (our last resort)
						if (_imu_accel_clipping_NE) {
							// accel clipping on horizontal axis

							// reset velocity (if current sample speed accuracy is good)
							if (gps_sample.sacc < _params.req_sacc) {
								_information_events.flags.reset_vel_to_gps = true;
								resetVelocityTo(gps_sample.vel, vel_obs_var);
								_aid_src_gnss_vel.time_last_fuse = _imu_sample_delayed.time_us;

								_last_gnss_reset_accel_clipping_us = _imu_sample_delayed.time_us;
							}

							// reset position (if current sample horizontal accuracy is good)
							if (gps_sample.hacc < _params.req_hacc) {
								_information_events.flags.reset_pos_to_gps = true;
								resetHorizontalPositionTo(gps_sample.pos, pos_obs_var);
								_aid_src_gnss_pos.time_last_fuse = _imu_sample_delayed.time_us;

								_last_gnss_reset_accel_clipping_us = _imu_sample_delayed.time_us;
							}

						} else if (_imu_accel_clipping_D || isHeightResetRequired()) {
							// accel clipping on vertical axis or height failure, reset vertical velocity
							if (gps_sample.sacc < _params.req_sacc) {
								resetVerticalVelocityTo(gps_sample.vel(2), vel_obs_var(2));

								_last_gnss_reset_accel_clipping_us = _imu_sample_delayed.time_us;
							}
						}
					}

				} else {
					stopGpsFusion();
					_warning_events.flags.gps_quality_poor = true;
					ECL_WARN("GPS quality poor - stopping use");

					// TODO: move this to EV control logic
					// Reset position state to external vision if we are going to use absolute values
					if (_control_status.flags.ev_pos && !(_params.fusion_mode & SensorFusionMask::ROTATE_EXT_VIS)) {
						resetHorizontalPositionToVision();
					}
				}

			} else { // mandatory conditions are not passing
				stopGpsFusion();
			}

		} else {
			if (starting_conditions_passing) {
				// Do not use external vision for yaw if using GPS because yaw needs to be
				// defined relative to an NED reference frame
				if (_control_status.flags.ev_yaw
				    || _mag_inhibit_yaw_reset_req
				    || _mag_yaw_reset_req) {

					_mag_yaw_reset_req = true;

					// Stop the vision for yaw fusion and do not allow it to start again
					stopEvYawFusion();
					_inhibit_ev_yaw_use = true;

				} else {
					ECL_INFO("starting GPS fusion");
					_information_events.flags.starting_gps_fusion = true;

					// reset position
					_information_events.flags.reset_pos_to_gps = true;
					resetHorizontalPositionTo(gps_sample.pos, pos_obs_var);

					// when already using another velocity source velocity reset is not necessary
					if (!isHorizontalAidingActive()) {
						_information_events.flags.reset_vel_to_gps = true;
						resetVelocityTo(gps_sample.vel, vel_obs_var);
					}

					_control_status.flags.gps = true;
				}

			} else if (gps_checks_passing && !_control_status.flags.yaw_align && (_params.mag_fusion_type == MagFuseType::NONE)) {
				// If no mag is used, align using the yaw estimator (if available)
				if (resetYawToEKFGSF()) {
					_information_events.flags.yaw_aligned_to_imu_gps = true;
					ECL_INFO("Yaw aligned using IMU and GPS");

					ECL_INFO("reset velocity and position to GPS");
					_information_events.flags.reset_vel_to_gps = true;
					_information_events.flags.reset_pos_to_gps = true;
					resetVelocityTo(gps_sample.vel, vel_obs_var);
					resetHorizontalPositionTo(gps_sample.pos, pos_obs_var);
				}
			}
		}

	} else if (_control_status.flags.gps && !isNewestSampleRecent(_time_last_gps_buffer_push, (uint64_t)10e6)) {
		stopGpsFusion();
		_warning_events.flags.gps_data_stopped = true;
		ECL_WARN("GPS data stopped");

	}  else if (_control_status.flags.gps && !isNewestSampleRecent(_time_last_gps_buffer_push, (uint64_t)1e6)
		    && isOtherSourceOfHorizontalAidingThan(_control_status.flags.gps)) {

		// Handle the case where we are fusing another position source along GPS,
		// stop waiting for GPS after 1 s of lost signal
		stopGpsFusion();
		_warning_events.flags.gps_data_stopped_using_alternate = true;
		ECL_WARN("GPS data stopped, using only EV, OF or air data");
	}
}

bool Ekf::shouldResetGpsFusion() const
{
	/* We are relying on aiding to constrain drift so after a specified time
	 * with no aiding we need to do something
	 */
	const bool has_horizontal_aiding_timed_out = isTimedOut(_time_last_hor_pos_fuse, _params.reset_timeout_max)
						     && isTimedOut(_time_last_hor_vel_fuse, _params.reset_timeout_max)
						     && isTimedOut(_aid_src_optical_flow.time_last_fuse, _params.reset_timeout_max);

	const bool is_reset_required = has_horizontal_aiding_timed_out
				       || isTimedOut(_time_last_hor_pos_fuse, 2 * _params.reset_timeout_max);

	const bool is_inflight_nav_failure = _control_status.flags.in_air
					     && isTimedOut(_time_last_hor_vel_fuse, _params.reset_timeout_max)
					     && isTimedOut(_time_last_hor_pos_fuse, _params.reset_timeout_max)
					     && (_time_last_hor_vel_fuse > _time_last_on_ground_us)
					     && (_time_last_hor_pos_fuse > _time_last_on_ground_us);

	return (is_reset_required || is_inflight_nav_failure);
}

bool Ekf::isYawFailure() const
{
	if (!isYawEmergencyEstimateAvailable()) {
		return false;
	}

	const float euler_yaw = getEulerYaw(_R_to_earth);
	const float yaw_error = wrap_pi(euler_yaw - _yawEstimator.getYaw());

	return fabsf(yaw_error) > math::radians(25.f);
}
