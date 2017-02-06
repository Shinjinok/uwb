/****************************************************************************
 *
 *   Copyright (c) 2013-2016 PX4 Development Team. All rights reserved.
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

/*
 * @file PreclandBeaconEst.h
Land detector interface for multicopter, fixedwing and VTOL implementations.
 *
 * @author Johan Jansen <jnsn.johan@gmail.com>
 * @author Julian Oes <julian@oes.ch>
 */

#pragma once

#include <px4_workqueue.h>
#include <systemlib/param/param.h>
#include <uORB/uORB.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/irlock_report.h>
#include <uORB/topics/precland_beacon_relpos.h>
#include <uORB/topics/parameter_update.h>
#include <matrix/math.hpp>
#include <mathlib/mathlib.h>
#include <matrix/Matrix.hpp>


namespace precland_beacon_est
{


class PreclandBeaconEst
{
public:

	PreclandBeaconEst();
	virtual ~PreclandBeaconEst();

	/*
	 * @return true if this task is currently running.
	 */
	inline bool is_running() const
	{
		return _taskIsRunning;
	}

	/*
	 * Tells the task that it should exit.
	 */
	void stop();

	/*
	 * Get the work queue going.
	 */
	int start();

protected:
	/*
	 * Called once to initialize uORB topics.
	 */
	void _initialize_topics();

	/*
	 * Update uORB topics.
	 */
	void _update_topics();

	/*
	 * Update parameters.
	 */
	void _update_params();

	/*
	 * Convenience function for polling uORB subscriptions.
	 *
	 * @return true if there was new data and it was successfully copied
	 */
	static bool _orb_update(const struct orb_metadata *meta, int handle, void *buffer);

	/* Run main land detector loop at this rate in Hz. */
	static constexpr uint32_t PRECLAND_BEACON_EST_UPDATE_RATE_HZ = 50;

	orb_advert_t _preclandBeaconRelposPub;
	struct precland_beacon_relpos_s _preclandBeaconRelpos;

	int _parameterSub;

private:
	int _vehicleLocalPositionSub;
	int _attitudeSub;
	int _irlockReportSub;

	struct vehicle_local_position_s		_vehicleLocalPosition;
	struct vehicle_attitude_s			_vehicleAttitude;
	struct irlock_report_s				_irlockReport;

	// keep track of which topics we have received
	bool _new_vehicleLocalPosition;
	bool _new_vehicleAttitude;
	bool _new_irlockReport;

	matrix::Dcm<float> _R_att;

	static void _cycle_trampoline(void *arg);

	void _cycle();

	void _check_params(const bool force);

	void _update_state();

	bool _taskShouldExit;
	bool _taskIsRunning;

	struct work_s	_work;
};


} // namespace precland_beacon_est
