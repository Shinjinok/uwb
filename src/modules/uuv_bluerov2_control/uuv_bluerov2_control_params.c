/****************************************************************************
 *
 *   Copyright (c) 2013-2020 PX4 Development Team. All rights reserved.
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
 * AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
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
 * @file uuv_bluerov2_control_params.c
 *
 * Parameters defined by the attitude control task for unmanned underwater vehicles (UUVs)
 *
 * This is a modification of the fixed wing/ground rover params and it is designed for ground rovers.
 * It has been developed starting from the fw  module, simplified and improved with dedicated items.
 *
 * All the ackowledgments and credits for the fw wing/rover app are reported in those files.
 *
 * @author Daniel Duecker <daniel.duecker@tuhh.de>
 */

/*
 * Controller parameters, accessible via MAVLink
 *
 */

// Roll gains
/**
 * Roll proportional gain
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_ROLL_P, 1.0f);

/**
 * Roll differential gain
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_ROLL_D, 0.2f);


// Pitch gains
/**
 * Pitch proportional gain
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_PITCH_P, 1.0f);

/**
 * Pitch differential gain
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_PITCH_D, 0.2f);


// Yaw gains
/**
 * Yawh proportional gain
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_YAW_P, 1.0f);

/**
 * Yaw differential gain
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_YAW_D, 0.2f);


// Input Modes
/**
 * Select Input Mode
 *
 * @value 0 use Attitude Setpoints
 * @value 1 Direct Feedthrough
 * @group UUV Attitude Control
 */
PARAM_DEFINE_INT32(BR2_INPUT_MODE, 0);

/**
 * Skip the controller
 *
 * @value 0 use the module's controller
 * @value 1 skip the controller and feedthrough the setpoints
 */
PARAM_DEFINE_INT32(BR2_SKIP_CTRL, 0);

/**
 * Direct roll input
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_D_ROLL, 0.0f);

/**
 * Direct pitch input
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_D_PITCH, 0.0f);

/**
 * Direct yaw input
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_D_YAW, 0.0f);

/**
 * Direct X thrust input
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_D_T_X, 0.0f);

/**
 * Direct Y thrust input
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_D_T_Y, 0.0f);

/**
 * Direct desired DEPTH input
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_DES_DEPTH, 0.2f);

/**
 * Depth P Gain
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_DEPTH_P, 1.0f);
/**
 * Depth D Gain
 *
 * @group UUV Attitude Control
 */
PARAM_DEFINE_FLOAT(BR2_DEPTH_D, 0.2f);
