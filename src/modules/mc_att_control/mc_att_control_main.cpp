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
 * @file mc_att_control_main.cpp
 * Multicopter attitude controller. 多旋翼姿态控制器
 *
 * Publication for the desired attitude tracking:
 * 
 * Daniel Mellinger and Vijay Kumar. Minimum Snap Trajectory Generation and Control for Quadrotors.
 * Int. Conf. on Robotics and Automation, Shanghai, China, May 2011.
 *
 * @author Lorenz Meier <lorenz@px4.io>
 * @author Anton Babushkin <anton.babushkin@me.com>
 * The controller has two loops: P loop for angular error and PD loop for angular rate error.
 * 控制器有两个循环: 角度误差的P回路与角速度误差的PD回路
 * Desired rotation calculated keeping in mind that yaw response is normally slower than roll/pitch.
 * 偏航响应比横滚/俯仰慢
 * For small deviations controller rotates copter to have shortest path of thrust vector and independently rotates around yaw,
 * 对于小的偏差，控制器旋转飞行器使推力向量以最短的路径独立的绕横滚轴转动
 * so actual rotation axis is not constant. For large deviations controller rotates copter around fixed axis.
 * 实际转动轴不是常量。对于大的偏差控制器绕固定轴转动飞行器
 * These two approaches fused seamlessly with weight depending on angular error.
 * 这两个方法根据角度误差无缝融合
 * When thrust vector directed near-horizontally (e.g. roll ~= PI/2) yaw setpoint ignored because of singularity.
 * 当推力向量指向水平，偏航设点可以忽略，因为奇点问题   (万向锁)
 * Controller doesn't use Euler angles for work, they generated only for more human-friendly control and logging.
 * 控制器不用欧拉角工作，欧拉角只是用来更加人性化的控制与记录，因为欧拉角很直观，与生活相近
 * If rotation matrix setpoint is invalid it will be generated from Euler angles for compatibility with old position controllers.
 * 如果旋转矩阵设点无效，将用欧拉角生成矩阵，以兼容旧的位置控制器
 */

#include <px4_config.h>
#include <px4_defines.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <drivers/drv_hrt.h>
#include <arch/board/board.h>
#include <uORB/uORB.h>
#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/manual_control_setpoint.h>
#include <uORB/topics/actuator_controls.h>
#include <uORB/topics/vehicle_rates_setpoint.h>
#include <uORB/topics/fw_virtual_rates_setpoint.h>
#include <uORB/topics/mc_virtual_rates_setpoint.h>
#include <uORB/topics/control_state.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/actuator_armed.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/multirotor_motor_limits.h>
#include <uORB/topics/mc_att_ctrl_status.h>
#include <systemlib/param/param.h>
#include <systemlib/err.h>
#include <systemlib/perf_counter.h>
#include <systemlib/systemlib.h>
#include <systemlib/circuit_breaker.h>
#include <lib/mathlib/mathlib.h>
#include <lib/geo/geo.h>
#include <lib/tailsitter_recovery/tailsitter_recovery.h>

/**
 * Multicopter attitude control app start / stop handling function
 * 多旋翼姿态控制应用 开始/结束 处理函数
 * @ingroup apps
 */
extern "C" __EXPORT int mc_att_control_main(int argc, char *argv[]);

#define YAW_DEADZONE	0.05f
#define MIN_TAKEOFF_THRUST    0.2f
#define RATES_I_LIMIT	0.3f
#define MANUAL_THROTTLE_MAX_MULTICOPTER	0.9f
#define ATTITUDE_TC_DEFAULT 0.2f

#define AXIS_INDEX_ROLL 0
#define AXIS_INDEX_PITCH 1
#define AXIS_INDEX_YAW 2
#define AXIS_COUNT 3

class MulticopterAttitudeControl
{
public:
	/**
	 * Constructor
	 */
	MulticopterAttitudeControl();

	/**
	 * Destructor, also kills the main task
	 */
	~MulticopterAttitudeControl();

	/**
	 * Start the multicopter attitude control task.
	 *
	 * @return		OK on success.
	 */
	int		start();

private:

	bool	_task_should_exit;		/**< if true, task_main() should exit  */
	  // 如果为真，task_main()退出
	int		_control_task;			/**< task handle 任务句柄 */

	int		_ctrl_state_sub;		/**< control state subscription  订阅控制状态*/
	int		_v_att_sp_sub;			/**< vehicle attitude setpoint subscription  订阅飞行器姿态设定值*/
	int		_v_rates_sp_sub;		/**< vehicle rates setpoint subscription 订阅飞行器的速度设定值*/
	int		_v_control_mode_sub;	/**< vehicle control mode subscription  订阅飞机的控制模式*/
	int		_params_sub;			/**< parameter updates subscription  订阅参数更新 */
	int		_manual_control_sp_sub;	/**< manual control setpoint subscription 订阅手动控制设定值*/
	int		_armed_sub;				/**< arming status subscription */
	int		_vehicle_status_sub;	    /**< vehicle status subscription */
	int 	_motor_limits_sub;		/**< motor limits subscription */

	orb_advert_t	_v_rates_sp_pub;		/**< rate setpoint publication */
	orb_advert_t	_actuators_0_pub;		/**< attitude actuator controls publication */
	orb_advert_t	_controller_status_pub;	/**< controller status publication */

	orb_id_t _rates_sp_id;	   /**< pointer to correct rates setpoint uORB metadata structure */
							   // uORB元数据结构体矫正速度设定值的指针
							   
	orb_id_t _actuators_id;	/**< pointer to correct actuator controls0 uORB metadata structure */

	bool		_actuators_0_circuit_breaker_enabled;	/**< circuit breaker to suppress output */
															// 抑制输出的断路器

	struct control_state_s				_ctrl_state;		/**< control state 控制状态*/
	struct vehicle_attitude_setpoint_s	_v_att_sp;			/**< vehicle attitude setpoint 飞行器姿态设定值 */
	struct vehicle_rates_setpoint_s		_v_rates_sp;		/**< vehicle rates setpoint 飞行器速度设定值*/
	struct manual_control_setpoint_s	_manual_control_sp;	/**< manual control setpoint */
	struct vehicle_control_mode_s		_v_control_mode;	    /**< vehicle control mode */
	struct actuator_controls_s			_actuators;			/**< actuator controls */
	struct actuator_armed_s				_armed;				/**< actuator arming status */
	struct vehicle_status_s				_vehicle_status;	/**< vehicle status */
	struct multirotor_motor_limits_s	_motor_limits;		/**< motor limits */
	struct mc_att_ctrl_status_s 		_controller_status; /**< controller status 控制器状态*/

	perf_counter_t	_loop_perf;			/**< loop performance counter */
	perf_counter_t	_controller_latency_perf;

	math::Vector<3>		_rates_prev;		/**< angular rates on previous step 前一步的角速度 */
	math::Vector<3>		_rates_sp_prev;   /**< previous rates setpoint 之前的角速度设定值*/
	math::Vector<3>		_rates_sp;			/**< angular rates setpoint 角速度设定值*/
	math::Vector<3>		_rates_int;		/**< angular rates integral error 角速度积分误差*/
	float				_thrust_sp;		/**< thrust setpoint 推力设定值*/
	math::Vector<3>		_att_control;		/**< attitude control vector 姿态控制向量-欧拉角*/

	math::Matrix<3, 3>  _I;					/**< identity matrix 单位矩阵*/

	struct {
		param_t roll_p;
		param_t roll_rate_p;
		param_t roll_rate_i;
		param_t roll_rate_d;
		param_t roll_rate_ff;
		param_t pitch_p;
		param_t pitch_rate_p;
		param_t pitch_rate_i;
		param_t pitch_rate_d;
		param_t pitch_rate_ff;
		param_t tpa_breakpoint;
		param_t tpa_slope;
		param_t yaw_p;
		param_t yaw_rate_p;
		param_t yaw_rate_i;
		param_t yaw_rate_d;
		param_t yaw_rate_ff;
		param_t yaw_ff;
		param_t roll_rate_max;
		param_t pitch_rate_max;
		param_t yaw_rate_max;

		param_t acro_roll_max;
		param_t acro_pitch_max;
		param_t acro_yaw_max;
		param_t rattitude_thres;

		param_t vtol_type;
		param_t roll_tc;
		param_t pitch_tc;

	}		_params_handles;		/**< handles for interesting parameters  */
															   // 感兴趣的参数的句柄
	struct {
		math::Vector<3> att_p;					/**< P gain for angular error  角度误差的P增益*/
		math::Vector<3> rate_p;				/**< P gain for angular rate error 角速度误差的P增益*/
		math::Vector<3> rate_i;				/**< I gain for angular rate error 角速度误差的I增益*/
		math::Vector<3> rate_d;				/**< D gain for angular rate error 角速度误差的D增益*/
		math::Vector<3>	rate_ff;			/**< Feedforward gain for desired rates 期望速度的前馈增益*/
		float yaw_ff;						/**< yaw control feed-forward 偏航控制的前馈*/

		float tpa_breakpoint;				/**< Throttle PID Attenuation breakpoint */
		float tpa_slope;					/**< Throttle PID Attenuation slope */

		float roll_rate_max;
		float pitch_rate_max;
		float yaw_rate_max;
		math::Vector<3> mc_rate_max;		/**< attitude rate limits in stabilized modes */
											// 自稳模式下的角速度

		math::Vector<3> acro_rate_max;		/**< max attitude rates in acro mode  */
											// 特技模式下的最大速度
		float rattitude_thres;
		int vtol_type;						/**< 0 = Tailsitter, 1 = Tiltrotor, 2 = Standard airframe */
	}		_params;

	TailsitterRecovery *_ts_opt_recovery;	/**< Computes optimal rates for tailsitter recovery */
											//计算tailsitter的最优速度

	/**
	 * Update our local parameter cache.
	 */
	int			parameters_update();

	/**
	 * Check for parameter update and handle it.
	 */
	void		parameter_update_poll();

	/**
	 * Check for changes in vehicle control mode.
	 */
	void		vehicle_control_mode_poll();

	/**
	 * Check for changes in manual inputs.
	 */
	void		vehicle_manual_poll();

	/**
	 * Check for attitude setpoint updates.
	 */
	void		vehicle_attitude_setpoint_poll();

	/**
	 * Check for rates setpoint updates.
	 */
	void		vehicle_rates_setpoint_poll();

	/**
	 * Check for arming status updates.
	 */
	void		arming_status_poll();

	/**
	 * Attitude controller. 姿态控制器
	 */
	void		control_attitude(float dt);

	/**
	 * Attitude rates controller.
	 */
	void		control_attitude_rates(float dt);

	/**
	 * Check for vehicle status updates.
	 */
	void		vehicle_status_poll();

	/**
	 * Check for vehicle motor limits status.
	 */
	void		vehicle_motor_limits_poll();

	/**
	 * Shim for calling task_main from task_create.
	 */
	static void	task_main_trampoline(int argc, char *argv[]);

	/**
	 * Main attitude control task.
	 */
	void		task_main();
};

namespace mc_att_control
{

MulticopterAttitudeControl	*g_control;
}

MulticopterAttitudeControl::MulticopterAttitudeControl() :

	_task_should_exit(false),
	_control_task(-1),

	/* subscriptions */
	_ctrl_state_sub(-1),
	_v_att_sp_sub(-1),
	_v_control_mode_sub(-1),
	_params_sub(-1),
	_manual_control_sp_sub(-1),
	_armed_sub(-1),
	_vehicle_status_sub(-1),

	/* publications */
	_v_rates_sp_pub(nullptr),
	_actuators_0_pub(nullptr),
	_controller_status_pub(nullptr),
	_rates_sp_id(0),
	_actuators_id(0),

	_actuators_0_circuit_breaker_enabled(false),

	/* performance counters */
	_loop_perf(perf_alloc(PC_ELAPSED, "mc_att_control")),
	_controller_latency_perf(perf_alloc_once(PC_ELAPSED, "ctrl_latency")),
	_ts_opt_recovery(nullptr)

{
	memset(&_ctrl_state, 0, sizeof(_ctrl_state));
	memset(&_v_att_sp, 0, sizeof(_v_att_sp));
	memset(&_v_rates_sp, 0, sizeof(_v_rates_sp));
	memset(&_manual_control_sp, 0, sizeof(_manual_control_sp));
	memset(&_v_control_mode, 0, sizeof(_v_control_mode));
	memset(&_actuators, 0, sizeof(_actuators));
	memset(&_armed, 0, sizeof(_armed));
	memset(&_vehicle_status, 0, sizeof(_vehicle_status));
	memset(&_motor_limits, 0, sizeof(_motor_limits));
	memset(&_controller_status, 0, sizeof(_controller_status));
	_vehicle_status.is_rotary_wing = true;

	_params.att_p.zero();
	_params.rate_p.zero();
	_params.rate_i.zero();
	_params.rate_d.zero();
	_params.rate_ff.zero();
	_params.yaw_ff = 0.0f;
	_params.roll_rate_max = 0.0f;
	_params.pitch_rate_max = 0.0f;
	_params.yaw_rate_max = 0.0f;
	_params.mc_rate_max.zero();
	_params.acro_rate_max.zero();
	_params.rattitude_thres = 1.0f;

	_rates_prev.zero();
	_rates_sp.zero();
	_rates_sp_prev.zero();
	_rates_int.zero();
	_thrust_sp = 0.0f;
	_att_control.zero();

	_I.identity();

	_params_handles.roll_p			= 	param_find("MC_ROLL_P");
	_params_handles.roll_rate_p		= 	param_find("MC_ROLLRATE_P");
	_params_handles.roll_rate_i		= 	param_find("MC_ROLLRATE_I");
	_params_handles.roll_rate_d		= 	param_find("MC_ROLLRATE_D");
	_params_handles.roll_rate_ff	= 	param_find("MC_ROLLRATE_FF");
	_params_handles.pitch_p			= 	param_find("MC_PITCH_P");
	_params_handles.pitch_rate_p	= 	param_find("MC_PITCHRATE_P");
	_params_handles.pitch_rate_i	= 	param_find("MC_PITCHRATE_I");
	_params_handles.pitch_rate_d	= 	param_find("MC_PITCHRATE_D");
	_params_handles.pitch_rate_ff 	= 	param_find("MC_PITCHRATE_FF");
	_params_handles.tpa_breakpoint 	= 	param_find("MC_TPA_BREAK");
	_params_handles.tpa_slope	 	= 	param_find("MC_TPA_SLOPE");
	_params_handles.yaw_p			=	param_find("MC_YAW_P");
	_params_handles.yaw_rate_p		= 	param_find("MC_YAWRATE_P");
	_params_handles.yaw_rate_i		= 	param_find("MC_YAWRATE_I");
	_params_handles.yaw_rate_d		= 	param_find("MC_YAWRATE_D");
	_params_handles.yaw_rate_ff	 	= 	param_find("MC_YAWRATE_FF");
	_params_handles.yaw_ff			= 	param_find("MC_YAW_FF");
	_params_handles.roll_rate_max	= 	param_find("MC_ROLLRATE_MAX");
	_params_handles.pitch_rate_max	= 	param_find("MC_PITCHRATE_MAX");
	_params_handles.yaw_rate_max	= 	param_find("MC_YAWRATE_MAX");
	_params_handles.acro_roll_max	= 	param_find("MC_ACRO_R_MAX");
	_params_handles.acro_pitch_max	= 	param_find("MC_ACRO_P_MAX");
	_params_handles.acro_yaw_max	= 	param_find("MC_ACRO_Y_MAX");
	_params_handles.rattitude_thres = 	param_find("MC_RATT_TH");
	_params_handles.vtol_type 		= 	param_find("VT_TYPE");
	_params_handles.roll_tc			= 	param_find("MC_ROLL_TC");
	_params_handles.pitch_tc		= 	param_find("MC_PITCH_TC");

	/* fetch initial parameter values */
	parameters_update();

	if (_params.vtol_type == 0) {
		// the vehicle is a tailsitter, use optimal recovery control strategy
		_ts_opt_recovery = new TailsitterRecovery();
	}


}

MulticopterAttitudeControl::~MulticopterAttitudeControl()
{
	if (_control_task != -1) {
		/* task wakes up every 100ms or so at the longest */
		_task_should_exit = true;

		/* wait for a second for the task to quit at our request */
		unsigned i = 0;

		do {
			/* wait 20ms */
			usleep(20000);

			/* if we have given up, kill it */
			if (++i > 50) {
				px4_task_delete(_control_task);
				break;
			}
		} while (_control_task != -1);
	}
<<<<<<< fd2be75d086e81726495197a6c8cb7a75ee1a45e

	if (_ts_opt_recovery != nullptr) {
		delete _ts_opt_recovery;
	}
=======
>>>>>>> let's start

	delete _ts_opt_recovery;
	mc_att_control::g_control = nullptr;
}

int
MulticopterAttitudeControl::parameters_update()
{
	float v;

	float roll_tc, pitch_tc;

	param_get(_params_handles.roll_tc, &roll_tc);
	param_get(_params_handles.pitch_tc, &pitch_tc);

	/* roll gains 横滚增益 */
	param_get(_params_handles.roll_p, &v);
	_params.att_p(0) = v * (ATTITUDE_TC_DEFAULT / roll_tc);
	param_get(_params_handles.roll_rate_p, &v);
	_params.rate_p(0) = v * (ATTITUDE_TC_DEFAULT / roll_tc);
	param_get(_params_handles.roll_rate_i, &v);
	_params.rate_i(0) = v;
	param_get(_params_handles.roll_rate_d, &v);
	_params.rate_d(0) = v * (ATTITUDE_TC_DEFAULT / roll_tc);
	param_get(_params_handles.roll_rate_ff, &v);
	_params.rate_ff(0) = v;

	/* pitch gains */
	param_get(_params_handles.pitch_p, &v);
	_params.att_p(1) = v * (ATTITUDE_TC_DEFAULT / pitch_tc);
	param_get(_params_handles.pitch_rate_p, &v);
	_params.rate_p(1) = v * (ATTITUDE_TC_DEFAULT / pitch_tc);
	param_get(_params_handles.pitch_rate_i, &v);
	_params.rate_i(1) = v;
	param_get(_params_handles.pitch_rate_d, &v);
	_params.rate_d(1) = v * (ATTITUDE_TC_DEFAULT / pitch_tc);
	param_get(_params_handles.pitch_rate_ff, &v);
	_params.rate_ff(1) = v;

	param_get(_params_handles.tpa_breakpoint, &v);
	_params.tpa_breakpoint = v;
	param_get(_params_handles.tpa_slope, &v);
	_params.tpa_slope = v;

	/* yaw gains */
	param_get(_params_handles.yaw_p, &v);
	_params.att_p(2) = v;
	param_get(_params_handles.yaw_rate_p, &v);
	_params.rate_p(2) = v;
	param_get(_params_handles.yaw_rate_i, &v);
	_params.rate_i(2) = v;
	param_get(_params_handles.yaw_rate_d, &v);
	_params.rate_d(2) = v;
	param_get(_params_handles.yaw_rate_ff, &v);
	_params.rate_ff(2) = v;

	param_get(_params_handles.yaw_ff, &_params.yaw_ff);

	/* angular rate limits */
	param_get(_params_handles.roll_rate_max, &_params.roll_rate_max);
	_params.mc_rate_max(0) = math::radians(_params.roll_rate_max);
	param_get(_params_handles.pitch_rate_max, &_params.pitch_rate_max);
	_params.mc_rate_max(1) = math::radians(_params.pitch_rate_max);
	param_get(_params_handles.yaw_rate_max, &_params.yaw_rate_max);
	_params.mc_rate_max(2) = math::radians(_params.yaw_rate_max);

	/* manual rate control scale(比率控制模式-ACRO特技模式) and auto mode roll/pitch rate limits */
	param_get(_params_handles.acro_roll_max, &v);
	_params.acro_rate_max(0) = math::radians(v);
	param_get(_params_handles.acro_pitch_max, &v);
	_params.acro_rate_max(1) = math::radians(v);
	param_get(_params_handles.acro_yaw_max, &v);
	_params.acro_rate_max(2) = math::radians(v);

	/* stick deflection needed in rattitude mode to control rates not angles */
	// rAttitude模式下控制速度而非角度的 固定旋转
	param_get(_params_handles.rattitude_thres, &_params.rattitude_thres);

	param_get(_params_handles.vtol_type, &_params.vtol_type);

	_actuators_0_circuit_breaker_enabled = circuit_breaker_enabled("CBRK_RATE_CTRL", CBRK_RATE_CTRL_KEY);

	return OK;
}

void
MulticopterAttitudeControl::parameter_update_poll()
{
	bool updated;

	/* Check if parameters have changed */
	orb_check(_params_sub, &updated);

	if (updated) {
		struct parameter_update_s param_update;
		orb_copy(ORB_ID(parameter_update), _params_sub, &param_update);
		parameters_update();
	}
}

void
MulticopterAttitudeControl::vehicle_control_mode_poll()
{
	bool updated;

	/* Check if vehicle control mode has changed */
	orb_check(_v_control_mode_sub, &updated);

	if (updated) {
		orb_copy(ORB_ID(vehicle_control_mode), _v_control_mode_sub, &_v_control_mode);
	}
}

void
MulticopterAttitudeControl::vehicle_manual_poll()
{
	bool updated;

	/* get pilots inputs */
	orb_check(_manual_control_sp_sub, &updated);

	if (updated) {
		orb_copy(ORB_ID(manual_control_setpoint), _manual_control_sp_sub, &_manual_control_sp);
	}
}

void
MulticopterAttitudeControl::vehicle_attitude_setpoint_poll()
{
	/* check if there is a new setpoint */
	// 检查是否有新的设定值
	bool updated;
	orb_check(_v_att_sp_sub, &updated);

	if (updated) {
		orb_copy(ORB_ID(vehicle_attitude_setpoint), _v_att_sp_sub, &_v_att_sp);
	}
}

void
MulticopterAttitudeControl::vehicle_rates_setpoint_poll()
{
	/* check if there is a new setpoint */
	bool updated;
	orb_check(_v_rates_sp_sub, &updated);

	if (updated) {
		orb_copy(ORB_ID(vehicle_rates_setpoint), _v_rates_sp_sub, &_v_rates_sp);
	}
}

void
MulticopterAttitudeControl::arming_status_poll()
{
	/* check if there is a new setpoint */
	bool updated;
	orb_check(_armed_sub, &updated);

	if (updated) {
		orb_copy(ORB_ID(actuator_armed), _armed_sub, &_armed);
	}
}

void
MulticopterAttitudeControl::vehicle_status_poll()
{
	/* check if there is new status information */
	bool vehicle_status_updated;
	orb_check(_vehicle_status_sub, &vehicle_status_updated);

	if (vehicle_status_updated) {
		orb_copy(ORB_ID(vehicle_status), _vehicle_status_sub, &_vehicle_status);

		/* set correct uORB ID, depending on if vehicle is VTOL or not */
		if (!_rates_sp_id) {
			if (_vehicle_status.is_vtol) {
				_rates_sp_id = ORB_ID(mc_virtual_rates_setpoint);
				_actuators_id = ORB_ID(actuator_controls_virtual_mc);

			} else {
				_rates_sp_id = ORB_ID(vehicle_rates_setpoint);
				_actuators_id = ORB_ID(actuator_controls_0);
			}
		}
	}
}

void
MulticopterAttitudeControl::vehicle_motor_limits_poll()
{
	/* check if there is a new message */
	bool updated;
	orb_check(_motor_limits_sub, &updated);

	if (updated) {
		orb_copy(ORB_ID(multirotor_motor_limits), _motor_limits_sub, &_motor_limits);
	}
}

/**
 * Attitude controller.
 * Input: 'vehicle_attitude_setpoint' topics (depending on mode)
 * Output: '_rates_sp' vector, '_thrust_sp'
 */
 
 /*
 * 姿态控制器 即 欧拉角控制
 *输入: '飞行器的姿态设定值 vehicle_attitude_setpoint'话题(取决于模式)
 *输出:'角速度设定值rate_sp'向量， '油门设定值thrust_sp'
 */
 
void
MulticopterAttitudeControl::control_attitude(float dt)
{
	vehicle_attitude_setpoint_poll();

	_thrust_sp = _v_att_sp.thrust;

	/* construct attitude setpoint rotation matrix */
	// 构造姿态设定值旋转矩阵
	math::Matrix<3, 3> R_sp;
	R_sp.set(_v_att_sp.R_body);  //body-机体  该矩阵的各个元素是机体的欧拉角

	/* get current rotation matrix from control state quaternions */
	// 从控制状态四元数中取得当前旋转矩阵
	math::Quaternion q_att(_ctrl_state.q[0], _ctrl_state.q[1], _ctrl_state.q[2], _ctrl_state.q[3]);
	math::Matrix<3, 3> R = q_att.to_dcm(); //将控制状态四元数转换为DCM

	/* all input data is ready, run controller itself */
	// 所有的输入数据已经准备好了，运行控制器

	/* try to move thrust vector shortest way, because yaw response is slower than roll/pitch */
	// 尝试以最短的路径移动推力向量，因为偏航响应比横滚/俯仰慢
	// 取出两个矩阵的Z轴向量
	math::Vector<3> R_z(R(0, 2), R(1, 2), R(2, 2));
	math::Vector<3> R_sp_z(R_sp(0, 2), R_sp(1, 2), R_sp(2, 2));

	/* axis and sin(angle) of desired rotation */
	// 轴以及与期望旋转的角度正弦sin(angle)
	// 根据这两个Z轴计算出误差向量(参考坐标系)，并转换到机体坐标系
	math::Vector<3> e_R = R.transposed() * (R_z % R_sp_z);

	/* calculate angle error */
	// 计算角度误差
	/*
	*由公式a×b=|a||b|sin(theta)，a·b=|a||b|cos(theta)。
	*这里R_z和R_sp_z都是单位向量，模为1
	*误差向量e_R的模就是sin(theta)，点积就是cos(theta)
	*/
	float e_R_z_sin = e_R.length(); // length()计算向量的模 length-->平方根
	float e_R_z_cos = R_z * R_sp_z; // 夹角余弦

	/* calculate weight for yaw control */
	// 计算用于偏航控制的权重	
	float yaw_w = R_sp(2, 2) * R_sp(2, 2);

	/*因为多轴的yaw响应一般比roll/pitch慢，因此将两者解耦，先补偿roll/pitch的变化，计算R_rp*/
	/* calculate rotation matrix after roll/pitch only rotation */
	// 计算在仅进行横滚/俯仰旋转之后旋转矩阵
	math::Matrix<3, 3> R_rp;

	if (e_R_z_sin > 0.0f) {  //判断两个Z轴是否存在误差
		/* get axis-angle representation */
		// 取得轴-角之间的关系
		float e_R_z_angle = atan2f(e_R_z_sin, e_R_z_cos);
		math::Vector<3> e_R_z_axis = e_R / e_R_z_sin;

		e_R = e_R_z_axis * e_R_z_angle; //将e_R转成单位向量

		/* cross product matrix for e_R_axis */
		// 用于计算误差旋转矩阵轴e_R_axis的向量叉积矩阵 ？？�
		math::Matrix<3, 3> e_R_cp;
		e_R_cp.zero();
		e_R_cp(0, 1) = -e_R_z_axis(2);
		e_R_cp(0, 2) = e_R_z_axis(1);
		e_R_cp(1, 0) = e_R_z_axis(2);
		e_R_cp(1, 2) = -e_R_z_axis(0);
		e_R_cp(2, 0) = -e_R_z_axis(1);
		e_R_cp(2, 1) = e_R_z_axis(0);

		/* rotation matrix for roll/pitch only rotation */
		// 仅用于横滚/俯仰旋转的旋转矩阵
		R_rp = R * (_I + e_R_cp * e_R_z_sin + e_R_cp * e_R_cp * (1.0f - e_R_z_cos));
		// _I代表单位阵

	} else {
		/* zero roll/pitch rotation */
		// 0横滚/俯仰旋转
		R_rp = R;
	}

	/*现在Z轴已经重合了，只需要求yaw的误差角度*/

	/* R_rp and R_sp has the same Z axis, calculate yaw error */
	// R_sp是 姿态设定值旋转矩阵 sp=setpoint
	// R_rp和R_sp具有相同的Z轴。  计算偏航误差
	// 取出两个矩阵的X轴(现在只有x轴存在误差)
	math::Vector<3> R_sp_x(R_sp(0, 0), R_sp(1, 0), R_sp(2, 0));
	math::Vector<3> R_rp_x(R_rp(0, 0), R_rp(1, 0), R_rp(2, 0));
	//同样根据向量的叉积和点积求出误差角度的正弦和余弦，再反正切求出角度；
	e_R(2) = atan2f((R_rp_x % R_sp_x) * R_sp_z, R_rp_x * R_sp_x) * yaw_w;

	/*
	*以上旋转方法，适用于小角度的误差，当转动的角度偏大时，还需另一种方法；
	直接计算参考系到机体系的旋转矩阵，并转换成四元数形式
	*/
	if (e_R_z_cos < 0.0f) {
		/* for large thrust vector rotations use another rotation method:
<<<<<<< fd2be75d086e81726495197a6c8cb7a75ee1a45e
		 * calculate angle and axis for R -> R_sp rotation directly */
		math::Quaternion q_error;
		q_error.from_dcm(R.transposed() * R_sp);
		math::Vector<3> e_R_d = q_error(0) >= 0.0f ? q_error.imag()  * 2.0f : -q_error.imag() * 2.0f;
=======
		 * calculate angle and axis for R -> R_sp rotation directly 
		 * 对于大的推力向量使用其他旋转方法:计算角度以及轴直接作用于R->R_sp的旋转
		 */
		math::Quaternion q;
		q.from_dcm(R.transposed() * R_sp);
		math::Vector<3> e_R_d = q.imag(); //取出虚部
		e_R_d.normalize(); // 归一化
		e_R_d *= 2.0f * atan2f(e_R_d.length(), q(0)); //得到一个误差角度向量
>>>>>>> let's start

		/* use fusion of Z axis based rotation and direct rotation */
		// 使用Z轴融合基本旋转以及直接旋转
		float direct_w = e_R_z_cos * e_R_z_cos * yaw_w;  //求出互补系数
		// 更新e_R,包含两种旋转方法，互补；
		e_R = e_R * (1.0f - direct_w) + e_R_d * direct_w;  
	}

	/* calculate angular rates setpoint */
	// 计算角速度设定值
	// 求出角速度的期望值，供内环角速度控制使用
	_rates_sp = _params.att_p.emult(e_R);
	//emult: 两向量元素对应相乘data[i] = data[i] * v.data[i]

	/* limit rates 角速度限制*/
	for (int i = 0; i < 3; i++) {
<<<<<<< fd2be75d086e81726495197a6c8cb7a75ee1a45e
		if ((_v_control_mode.flag_control_velocity_enabled || _v_control_mode.flag_control_auto_enabled) &&
		    !_v_control_mode.flag_control_manual_enabled) {
			_rates_sp(i) = math::constrain(_rates_sp(i), -_params.auto_rate_max(i), _params.auto_rate_max(i));

		} else {
			_rates_sp(i) = math::constrain(_rates_sp(i), -_params.mc_rate_max(i), _params.mc_rate_max(i));
		}
=======
		_rates_sp(i) = math::constrain(_rates_sp(i), -_params.mc_rate_max(i), _params.mc_rate_max(i));
>>>>>>> let's start
	}

	/* feed forward yaw setpoint rate */
	// 偏航速度设定值的前馈
	// yaw响应较慢，因此再加入一个前馈控制
	_rates_sp(2) += _v_att_sp.yaw_sp_move_rate * yaw_w * _params.yaw_ff;
<<<<<<< fd2be75d086e81726495197a6c8cb7a75ee1a45e

	/* weather-vane mode, dampen yaw rate */
	if ((_v_control_mode.flag_control_velocity_enabled || _v_control_mode.flag_control_auto_enabled) &&
	    _v_att_sp.disable_mc_yaw_control == true && !_v_control_mode.flag_control_manual_enabled) {
		float wv_yaw_rate_max = _params.auto_rate_max(2) * _params.vtol_wv_yaw_rate_scale;
		_rates_sp(2) = math::constrain(_rates_sp(2), -wv_yaw_rate_max, wv_yaw_rate_max);
		// prevent integrator winding up in weathervane mode
		_rates_int(2) = 0.0f;
	}
=======
>>>>>>> let's start
}

/*
 * Attitude rates controller.
 * Input: '_rates_sp' vector, '_thrust_sp'
 * Output: '_att_control' vector
 */
/*
 * 角速度控制器
 * 输入:'角速度设定值 rate_sp'向量,'推力设定值thrust_sp'
 * 输出:'姿态控制向量 att_control'向量---欧拉角
 */
void
MulticopterAttitudeControl::control_attitude_rates(float dt)
{
	/* reset integral if disarmed */
	// 若未解锁则复位积分
	if (!_armed.armed || !_vehicle_status.is_rotary_wing) {
		_rates_int.zero();
	}

	/* current body angular rates */
	// 当前机身角速度
	math::Vector<3> rates;
	rates(0) = _ctrl_state.roll_rate;
	rates(1) = _ctrl_state.pitch_rate;
	rates(2) = _ctrl_state.yaw_rate;

	/* throttle pid attenuation factor */
	float tpa =  fmaxf(0.0f, fminf(1.0f, 1.0f - _params.tpa_slope * (fabsf(_v_rates_sp.thrust) - _params.tpa_breakpoint)));

	/* angular rates error */
	// 角速度误差 
	math::Vector<3> rates_err = _rates_sp - rates;
<<<<<<< fd2be75d086e81726495197a6c8cb7a75ee1a45e

	_att_control = _params.rate_p.emult(rates_err * tpa) + _params.rate_d.emult(_rates_prev - rates) / dt + _rates_int +
		       _params.rate_ff.emult(_rates_sp);

=======
	_att_control = _params.rate_p.emult(rates_err) + _params.rate_d.emult(_rates_prev - rates) / dt + _rates_int +
		       _params.rate_ff.emult(_rates_sp - _rates_sp_prev) / dt;  // 微分
>>>>>>> let's start
	_rates_sp_prev = _rates_sp;
	_rates_prev = rates;

	/* update integral only if not saturated on low limit and if motor commands are not saturated */
	// 仅当低限制不饱和以及电机命令不饱和时更新积分???
	if (_thrust_sp > MIN_TAKEOFF_THRUST && !_motor_limits.lower_limit && !_motor_limits.upper_limit) {
		for (int i = AXIS_INDEX_ROLL; i < AXIS_COUNT; i++) {
			if (fabsf(_att_control(i)) < _thrust_sp) {
				float rate_i = _rates_int(i) + _params.rate_i(i) * rates_err(i) * dt;

				if (PX4_ISFINITE(rate_i) && rate_i > -RATES_I_LIMIT && rate_i < RATES_I_LIMIT &&
				    _att_control(i) > -RATES_I_LIMIT && _att_control(i) < RATES_I_LIMIT &&
				    /* if the axis is the yaw axis, do not update the integral if the limit is hit */
				    !((i == AXIS_INDEX_YAW) && _motor_limits.yaw)) {
					_rates_int(i) = rate_i;
				}
			}
		}
	}
}

void
MulticopterAttitudeControl::task_main_trampoline(int argc, char *argv[])
{
	mc_att_control::g_control->task_main();
}

void
MulticopterAttitudeControl::task_main()
{

	/*
	 * do subscriptions
	 * 订阅
	 */
	_v_att_sp_sub = orb_subscribe(ORB_ID(vehicle_attitude_setpoint));
	_v_rates_sp_sub = orb_subscribe(ORB_ID(vehicle_rates_setpoint));
	_ctrl_state_sub = orb_subscribe(ORB_ID(control_state));
	_v_control_mode_sub = orb_subscribe(ORB_ID(vehicle_control_mode));
	_params_sub = orb_subscribe(ORB_ID(parameter_update));
	_manual_control_sp_sub = orb_subscribe(ORB_ID(manual_control_setpoint));
	_armed_sub = orb_subscribe(ORB_ID(actuator_armed));
	_vehicle_status_sub = orb_subscribe(ORB_ID(vehicle_status));
	_motor_limits_sub = orb_subscribe(ORB_ID(multirotor_motor_limits));

	/* initialize parameters cache 初始化参数缓存 */
	parameters_update();

	/* wakeup source: vehicle attitude 唤醒源:飞行器姿态*/
	px4_pollfd_struct_t fds[1];

	fds[0].fd = _ctrl_state_sub;
	fds[0].events = POLLIN;

	while (!_task_should_exit) {

		/* wait for up to 100ms for data 最多等100ms待数据*/
		int pret = px4_poll(&fds[0], (sizeof(fds) / sizeof(fds[0])), 100);

		/* timed out - periodic check for _task_should_exit 
		超时--周期性检测_task_should_exit是否退出 */
		if (pret == 0) {
			continue;
		}

		/* this is undesirable but not much we can do - might want to flag unhappy status
		这不是期望的结果但是我们也无能为力: 可能要标记不好的数据 */
		if (pret < 0) {
			warn("poll error %d, %d", pret, errno);
			/* sleep a bit before next try */
			usleep(100000);
			continue;
		}

		perf_begin(_loop_perf);

		/* run controller on attitude changes  在姿态改变上运行控制器*/
		if (fds[0].revents & POLLIN) {
			static uint64_t last_run = 0;
			float dt = (hrt_absolute_time() - last_run) / 1000000.0f;
			last_run = hrt_absolute_time();

			/* guard against too small (< 2ms) and too large (> 20ms) dt's */
			if (dt < 0.002f) {
				dt = 0.002f;

			} else if (dt > 0.02f) {
				dt = 0.02f;
			}

			/* copy attitude and control state topics 
			   复制姿态以及控制状态话题*/
			orb_copy(ORB_ID(control_state), _ctrl_state_sub, &_ctrl_state);

			/* check for updates in other topics 
				检查其他话题的更新*/
			parameter_update_poll();
			vehicle_control_mode_poll();
			arming_status_poll();
			vehicle_manual_poll();
			vehicle_status_poll();
			vehicle_motor_limits_poll();

			/* Check if we are in rattitude mode and the pilot is above the threshold on pitch
			 * or roll (yaw can rotate 360 in normal att control).  If both are true don't
<<<<<<< fd2be75d086e81726495197a6c8cb7a75ee1a45e
			 * even bother running the attitude controllers */
			if (_v_control_mode.flag_control_rattitude_enabled) {
=======
			 * even bother running the attitude controllers 
			 检查我们是否处于rAttitude模式以及飞行员(俯仰或者偏航(偏航在一般姿态控制下可以360度
			 旋转))是否在油门阈值之上。
			 如果两者都为真，那么不用运行姿态控制器*/
			if (_vehicle_status.main_state == vehicle_status_s::MAIN_STATE_RATTITUDE) {
>>>>>>> let's start
				if (fabsf(_manual_control_sp.y) > _params.rattitude_thres ||
				    fabsf(_manual_control_sp.x) > _params.rattitude_thres) {
					_v_control_mode.flag_control_attitude_enabled = false;
				}
			}

			if (_v_control_mode.flag_control_attitude_enabled) {

				if (_ts_opt_recovery == nullptr) {
					// the  tailsitter recovery instance has not been created, thus, the vehicle
					// is not a tailsitter, do normal attitude control
					// 若垂直起降恢复实例还没有被创建，那么飞行器就不是tailsitter
					//进行一般姿态控制
					control_attitude(dt);
					/*
					*请
					*注
					*意
					*看
					*这
					*里
					*/

				
				} else {
					vehicle_attitude_setpoint_poll();
					_thrust_sp = _v_att_sp.thrust;
					math::Quaternion q(_ctrl_state.q[0], _ctrl_state.q[1], _ctrl_state.q[2], _ctrl_state.q[3]);
					math::Quaternion q_sp(&_v_att_sp.q_d[0]);
					_ts_opt_recovery->setAttGains(_params.att_p, _params.yaw_ff);
					_ts_opt_recovery->calcOptimalRates(q, q_sp, _v_att_sp.yaw_sp_move_rate, _rates_sp);

					/* limit rates */
					for (int i = 0; i < 3; i++) {
						_rates_sp(i) = math::constrain(_rates_sp(i), -_params.mc_rate_max(i), _params.mc_rate_max(i));
					}
				}

				/* publish attitude rates setpoint
				   公布角速度设定值*/
				_v_rates_sp.roll = _rates_sp(0);
				_v_rates_sp.pitch = _rates_sp(1);
				_v_rates_sp.yaw = _rates_sp(2);
				_v_rates_sp.thrust = _thrust_sp;
				_v_rates_sp.timestamp = hrt_absolute_time();

				if (_v_rates_sp_pub != nullptr) {
					orb_publish(_rates_sp_id, _v_rates_sp_pub, &_v_rates_sp);

				} else if (_rates_sp_id) {
					_v_rates_sp_pub = orb_advertise(_rates_sp_id, &_v_rates_sp);
				}

				//}

			} else {
				/* attitude controller disabled, poll rates setpoint topic */
				// 禁用姿态控制器，监测速度设定值话题
				if (_v_control_mode.flag_control_manual_enabled) {
					/* manual rates control - ACRO mode */
					// 手动速度控制-特技模式
					_rates_sp = math::Vector<3>(_manual_control_sp.y, -_manual_control_sp.x,
								    _manual_control_sp.r).emult(_params.acro_rate_max);
					_thrust_sp = math::min(_manual_control_sp.z, MANUAL_THROTTLE_MAX_MULTICOPTER);

					/* publish attitude rates setpoint */
					_v_rates_sp.roll = _rates_sp(0);
					_v_rates_sp.pitch = _rates_sp(1);
					_v_rates_sp.yaw = _rates_sp(2);
					_v_rates_sp.thrust = _thrust_sp;
					_v_rates_sp.timestamp = hrt_absolute_time();

					if (_v_rates_sp_pub != nullptr) {
						orb_publish(_rates_sp_id, _v_rates_sp_pub, &_v_rates_sp);

					} else if (_rates_sp_id) {
						_v_rates_sp_pub = orb_advertise(_rates_sp_id, &_v_rates_sp);
					}

				} else {
					/* attitude controller disabled, poll rates setpoint topic */
					// 禁用姿态控制器，监测速度设定值话题
					vehicle_rates_setpoint_poll();
					_rates_sp(0) = _v_rates_sp.roll;
					_rates_sp(1) = _v_rates_sp.pitch;
					_rates_sp(2) = _v_rates_sp.yaw;
					_thrust_sp = _v_rates_sp.thrust;
				}
			}

			if (_v_control_mode.flag_control_rates_enabled) {
				control_attitude_rates(dt);

				/* publish actuator controls */
				_actuators.control[0] = (PX4_ISFINITE(_att_control(0))) ? _att_control(0) : 0.0f;
				_actuators.control[1] = (PX4_ISFINITE(_att_control(1))) ? _att_control(1) : 0.0f;
				_actuators.control[2] = (PX4_ISFINITE(_att_control(2))) ? _att_control(2) : 0.0f;
				_actuators.control[3] = (PX4_ISFINITE(_thrust_sp)) ? _thrust_sp : 0.0f;
				_actuators.timestamp = hrt_absolute_time();
				_actuators.timestamp_sample = _ctrl_state.timestamp;

				_controller_status.roll_rate_integ = _rates_int(0);
				_controller_status.pitch_rate_integ = _rates_int(1);
				_controller_status.yaw_rate_integ = _rates_int(2);
				_controller_status.timestamp = hrt_absolute_time();

				if (!_actuators_0_circuit_breaker_enabled) {
					if (_actuators_0_pub != nullptr) {
						orb_publish(_actuators_id, _actuators_0_pub, &_actuators);
						perf_end(_controller_latency_perf);

					} else if (_actuators_id) {
						_actuators_0_pub = orb_advertise(_actuators_id, &_actuators);
					}

				}

				/* publish controller status */
				// 公布控制器状态
				if (_controller_status_pub != nullptr) {
					orb_publish(ORB_ID(mc_att_ctrl_status), _controller_status_pub, &_controller_status);

				} else {
					_controller_status_pub = orb_advertise(ORB_ID(mc_att_ctrl_status), &_controller_status);
				}
			}
		}

		perf_end(_loop_perf);
	}

	_control_task = -1;
	return;
}

int
MulticopterAttitudeControl::start()
{
	ASSERT(_control_task == -1);

	/* start the task */
	_control_task = px4_task_spawn_cmd("mc_att_control",
					   SCHED_DEFAULT,
					   SCHED_PRIORITY_MAX - 5,
					   1500,
					   (px4_main_t)&MulticopterAttitudeControl::task_main_trampoline,
					   nullptr);

	if (_control_task < 0) {
		warn("task start failed");
		return -errno;
	}

	return OK;
}

int mc_att_control_main(int argc, char *argv[])
{
	if (argc < 2) {
		warnx("usage: mc_att_control {start|stop|status}");
		return 1;
	}

	if (!strcmp(argv[1], "start")) {

		if (mc_att_control::g_control != nullptr) {
			warnx("already running");
			return 1;
		}

		mc_att_control::g_control = new MulticopterAttitudeControl;

		if (mc_att_control::g_control == nullptr) {
			warnx("alloc failed");
			return 1;
		}

		if (OK != mc_att_control::g_control->start()) {
			delete mc_att_control::g_control;
			mc_att_control::g_control = nullptr;
			warnx("start failed");
			return 1;
		}

		return 0;
	}

	if (!strcmp(argv[1], "stop")) {
		if (mc_att_control::g_control == nullptr) {
			warnx("not running");
			return 1;
		}

		delete mc_att_control::g_control;
		mc_att_control::g_control = nullptr;
		return 0;
	}

	if (!strcmp(argv[1], "status")) {
		if (mc_att_control::g_control) {
			warnx("running");
			return 0;

		} else {
			warnx("not running");
			return 1;
		}
	}

	warnx("unrecognized command");
	return 1;
}
