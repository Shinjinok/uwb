/****************************************************************************
 *
 *   Copyright (c) 2013-2018 PX4 Development Team. All rights reserved.
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
 * @file LTEstOrientation.cpp
 * @brief Estimate the orientation of a target by processessing and fusing sensor data in a Kalman Filter.
 *
 * @author Jonas Perolini <jonspero@me.com>
 *
 */

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <drivers/drv_hrt.h>

#include "LTEstOrientation.h"

#define SEC2USEC 1000000.0f

namespace landing_target_estimator
{

using namespace matrix;

LTEstOrientation::LTEstOrientation() :
	ModuleParams(nullptr)
{
	_ltest_aid_ev_yaw_pub.advertise();
	_targetOrientationPub.advertise();

	_check_params(true);
}

LTEstOrientation::~LTEstOrientation()
{
	delete _target_estimator_orientation;
}

bool LTEstOrientation::init()
{

	_target_mode = (TargetMode)_param_ltest_mode.get();
	_ltest_TIMEOUT_US = (uint32_t)(_param_ltest_btout.get() * SEC2USEC);

	return selectTargetEstimator();
}

void LTEstOrientation::resetFilter()
{
	_estimator_initialized = false;
	_new_pos_sensor_acquired_time = 0;
}

void LTEstOrientation::update()
{
	_check_params(false);

	// predict the target state using a constant relative acceleration model
	if (_estimator_initialized) {

		if (hrt_absolute_time() - _last_update > _ltest_TIMEOUT_US) {
			PX4_WARN("LTE orientation estimator timeout");
			resetFilter();

		} else {
			predictionStep();
			_last_predict = hrt_absolute_time();
		}
	}

	// Update and fuse the observations and pulishes innovations
	if (update_step()) {
		_last_update = _last_predict;
	}

	if (_estimator_initialized) {publishTarget();}
}

bool LTEstOrientation::initEstimator(float theta_init)
{

	PX4_INFO("Theta init %.2f", (double)theta_init);

	_target_estimator_orientation->setPosition(theta_init);
	_target_estimator_orientation->setVelocity(0.f);
	_target_estimator_orientation->setStatePosVar(_yaw_unc);
	_target_estimator_orientation->setStateVelVar(_yaw_unc);

	return true;
}


void LTEstOrientation::predictionStep()
{
	// Time from last prediciton
	float dt = (hrt_absolute_time() - _last_predict) / SEC2USEC;

	_target_estimator_orientation->predictState(dt);
	_target_estimator_orientation->predictCov(dt);
}



bool LTEstOrientation::update_step()
{
	fiducial_marker_yaw_report_s fiducial_marker_orientation;
	targetObsOrientation obs_fiducial_marker_orientation;

	bool orientation_valid = false;

	// Process data from all topics
	if (_fiducial_marker_orientation_sub.update(&fiducial_marker_orientation)) {

		if (processObsVisionOrientation(fiducial_marker_orientation, obs_fiducial_marker_orientation)) {

			orientation_valid = ((hrt_absolute_time() - obs_fiducial_marker_orientation.timestamp) < measurement_valid_TIMEOUT_US);
		}
	}

	// If we have a new sensor and the estimator is initialized: fuse available measurements and publish innov.
	if (orientation_valid && _estimator_initialized) {

		return fuse_orientation(obs_fiducial_marker_orientation);

	} else if (orientation_valid && !_estimator_initialized) {

		// Wait 1 second before initilazing the estimator to have a velocity initial estimate.
		if (!_new_pos_sensor_acquired_time) {
			_new_pos_sensor_acquired_time = hrt_absolute_time();

		} else if ((hrt_absolute_time() - _new_pos_sensor_acquired_time) > 1000000) {

			float theta_init = obs_fiducial_marker_orientation.meas_theta;

			if (initEstimator(theta_init)) {
				PX4_INFO("LTE Orientation Estimator properly initialized.");
				_estimator_initialized = true;
				_last_update = hrt_absolute_time();
				_last_predict = _last_update;

			} else {
				resetFilter();
			}
		}
	}

	return false;
}


bool LTEstOrientation::processObsVisionOrientation(const fiducial_marker_yaw_report_s &fiducial_marker_orientation,
		targetObsOrientation &obs)
{

	const float vision_r_theta = wrap_pi(fiducial_marker_orientation.yaw_ned);

	float vision_r_theta_unc;

	if (_ev_noise_md) {
		vision_r_theta_unc = _range_sensor.valid ? (_ev_angle_noise * _ev_angle_noise * fmaxf(_range_sensor.dist_bottom,
				     1.f)) : (_ev_angle_noise * _ev_angle_noise * 10);

	} else {
		vision_r_theta_unc = fmaxf(fiducial_marker_orientation.yaw_var_ned, _ev_angle_noise * _ev_angle_noise);
	}

	hrt_abstime vision_timestamp = fiducial_marker_orientation.timestamp;

	/* ORIENTATION */
	if (!PX4_ISFINITE(vision_r_theta)) {
		PX4_WARN("VISION orientation is corrupt!");

	} else {

		obs.timestamp = vision_timestamp;
		obs.updated_theta = true;
		obs.meas_unc_theta = vision_r_theta_unc;
		obs.meas_theta = vision_r_theta;
		obs.meas_h_theta(0) = 1;

		return true;
	}

	return false;
}

bool LTEstOrientation::fuse_orientation(const targetObsOrientation &target_orientation_obs)
{
	// Update step for orientation
	bool meas_fused = false;

	estimator_aid_source_1d_s target_innov;

	// Compute the measurement's time delay (difference between state and measurement time on validity)
	const float dt_sync_us = (_last_predict - target_orientation_obs.timestamp);

	if (dt_sync_us > measurement_valid_TIMEOUT_US) {

		PX4_INFO("Orientation obs. rejected because too old. Time sync: %.2f [seconds] > timeout: %.2f [seconds]",
			 (double)(dt_sync_us / SEC2USEC), (double)(measurement_valid_TIMEOUT_US / SEC2USEC));

		// No measurement update, set to false
		target_innov.fusion_enabled = false;
		target_innov.fused = false;

	} else if (target_orientation_obs.updated_theta) {

		// Convert time sync to seconds
		const float dt_sync_s = dt_sync_us / SEC2USEC;

		_target_estimator_orientation->syncState(dt_sync_s);
		_target_estimator_orientation->setH(target_orientation_obs.meas_h_theta);
		target_innov.innovation_variance = _target_estimator_orientation->computeInnovCov(
				target_orientation_obs.meas_unc_theta);
		target_innov.innovation = _target_estimator_orientation->computeInnov(target_orientation_obs.meas_theta);
		meas_fused = _target_estimator_orientation->update();

		// Fill the target innovation field
		target_innov.fusion_enabled = true;
		target_innov.innovation_rejected = !meas_fused;
		target_innov.fused = meas_fused;

		target_innov.observation = target_orientation_obs.meas_theta;
		target_innov.observation_variance = target_orientation_obs.meas_unc_theta;
		target_innov.timestamp_sample = target_orientation_obs.timestamp;
		target_innov.timestamp = hrt_absolute_time();

	} else {
		// No yaw measurement
		target_innov.fusion_enabled = false;
		target_innov.fused = false;
	}

	_ltest_aid_ev_yaw_pub.publish(target_innov);

	return meas_fused;
}


void LTEstOrientation::publishTarget()
{
	landing_target_orientation_s target_orientation{};

	target_orientation.timestamp = _last_predict;
	target_orientation.orientation_valid = (hrt_absolute_time() - _last_update < landing_target_valid_TIMEOUT_US);

	target_orientation.theta = _target_estimator_orientation->getPosition();
	target_orientation.cov_theta =  _target_estimator_orientation->getPosVar();

	if (_target_mode == TargetMode::Moving || _target_mode == TargetMode::MovingAugmented) {
		target_orientation.v_theta = _target_estimator_orientation->getVelocity();
		target_orientation.cov_v_theta =  _target_estimator_orientation->getVelVar();
	}

	_targetOrientationPub.publish(target_orientation);
}

void LTEstOrientation::_check_params(const bool force)
{
	if (_parameter_update_sub.updated() || force) {
		parameter_update_s pupdate;
		_parameter_update_sub.copy(&pupdate);

		updateParams();
	}

	if (_range_sensor.valid) {
		_range_sensor.valid = (hrt_absolute_time() - _range_sensor.last_update) < measurement_updated_TIMEOUT_US;
	}
}

void LTEstOrientation::updateParams()
{

	ModuleParams::updateParams();

	_yaw_unc = _param_ltest_yaw_unc_in.get();
	_ev_angle_noise = _param_ltest_ev_angle_noise.get();
	_ev_noise_md = _param_ltest_ev_noise_md.get();
}

void LTEstOrientation::set_range_sensor(const float dist, const bool valid)
{
	_range_sensor.valid = valid;
	_range_sensor.dist_bottom = dist;
	_range_sensor.last_update = hrt_absolute_time();
}

bool LTEstOrientation::selectTargetEstimator()
{
	Base_KF_orientation *tmp_theta = nullptr;

	bool init_failed = true;

	if (_target_mode == TargetMode::Moving || _target_mode == TargetMode::MovingAugmented) {
		tmp_theta = new KF_orientation_moving;

	} else {
		tmp_theta = new KF_orientation_static;
	}

	init_failed = (tmp_theta == nullptr);

	if (init_failed) {
		PX4_ERR("LTE orientation init failed");
		return false;

	} else {

		delete _target_estimator_orientation;
		_target_estimator_orientation = tmp_theta;

		return true;
	}
}

} // namespace landing_target_estimator
