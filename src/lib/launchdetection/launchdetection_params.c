/****************************************************************************
 *
 *   Copyright (c) 2013, 2014 PX4 Development Team. All rights reserved.
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
 * @file launchdetection_params.c
 *
 * Parameters for launchdetection
 *
 * @author Thomas Gubler <thomasgubler@gmail.com>
 */

#include <px4_config.h>

#include <systemlib/param/param.h>

/*
 * Catapult launch detection parameters, accessible via MAVLink
 *
 */

/**
 * Enable launch detection.
 *
 * @min 0
 * @max 1
 * @group Launch detection
 */
PARAM_DEFINE_INT32(LAUN_ALL_ON, 0, LAUNCH_DETECTION_PARAMS);

/**
 * Catapult accelerometer theshold.
 *
 * LAUN_CAT_A * LAUN_CAT_T serves as threshold to trigger launch detection.
 *
 * @min 0
 * @group Launch detection
 */
PARAM_DEFINE_FLOAT(LAUN_CAT_A, 30.0f, LAUNCH_DETECTION_PARAMS);

/**
 * Catapult time theshold.
 *
 * LAUN_CAT_A * LAUN_CAT_T serves as threshold to trigger launch detection.
 *
 * @min 0
 * @group Launch detection
 */
PARAM_DEFINE_FLOAT(LAUN_CAT_T, 0.05f, LAUNCH_DETECTION_PARAMS);

/**
 * Motor delay
 *
 * Delay between starting attitude control and powering up the throttle (giving throttle control to the controller)
 * Before this timespan is up the throttle will be set to LAUN_THR_PRE, set to 0 to deactivate
 *
 * @unit seconds
 * @min 0
 * @group Launch detection
 */
PARAM_DEFINE_FLOAT(LAUN_CAT_MDEL, 0.0f, LAUNCH_DETECTION_PARAMS);

/**
 * Maximum pitch before the throttle is powered up (during motor delay phase)
 *
 * This is an extra limit for the maximum pitch which is imposed in the phase before the throttle turns on.
 * This allows to limit the maximum pitch angle during a bungee launch (make the launch less steep).
 *
 * @unit deg
 * @min 0
 * @max 45
 * @group Launch detection
 */
PARAM_DEFINE_FLOAT(LAUN_CAT_PMAX, 30.0f, LAUNCH_DETECTION_PARAMS);

/**
 * Throttle setting while detecting launch.
 *
 * The throttle is set to this value while the system is waiting for the take-off.
 *
 * @min 0
 * @max 1
 * @group Launch detection
 */
PARAM_DEFINE_FLOAT(LAUN_THR_PRE, 0.0f, LAUNCH_DETECTION_PARAMS);
