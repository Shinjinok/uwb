/****************************************************************************
 *
 *   Copyright (c) 2013-2015 PX4 Development Team. All rights reserved.
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
 * @file pwm_out_sim_params.c
 * Parameters for PWM output simulator.
 *
 * @author Pedro Roque, <padr@kth.se>
 */

/**
 * Maximum PWM Value
 *
 * Highest PWM value that can be output by the simulator.
 *
 * @min 0
 * @max 10000
 * @decimal 0
 * @increment 1
 * @group PWM Output Simulator
 */
PARAM_DEFINE_INT32(PWM_SIM_PWM_MAX, 2000);

/**
 * Minimum PWM Value
 *
 * Lowest PWM value that can be output by the simulator.
 *
 * @min -10000
 * @max 2000
 * @decimal 0
 * @increment 1
 * @group PWM Output Simulator
 */
PARAM_DEFINE_INT32(PWM_SIM_PWM_MIN, 1000);

/**
 * Failsafe PWM Value
 *
 * Output at Failsafe level.
 *
 * @min 0
 * @max 10000
 * @decimal 0
 * @increment 1
 * @group PWM Output Simulator
 */
PARAM_DEFINE_INT32(PWM_SIM_FAILSAFE, 600);

/**
 * Disarm PWM Value
 *
 * Output at Disarmed state.
 *
 * @min 0
 * @max 10000
 * @decimal 0
 * @increment 1
 * @group PWM Output Simulator
 */
PARAM_DEFINE_INT32(PWM_SIM_DISARM, 900);