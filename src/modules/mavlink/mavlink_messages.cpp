/****************************************************************************
 *
 *   Copyright (c) 2012-2022 PX4 Development Team. All rights reserved.
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
 * @file mavlink_messages.cpp
 * MAVLink 2.0 message formatters implementation.
 *
 * @author Lorenz Meier <lorenz@px4.io>
 * @author Anton Babushkin <anton.babushkin@me.com>
 */

#include "mavlink_main.h"
#include "mavlink_messages.h"
#include "mavlink_command_sender.h"
#include "mavlink_simple_analyzer.h"

#include <drivers/drv_pwm_output.h>
#include <lib/conversion/rotation.h>
#include <lib/geo/geo.h>
#include <lib/mathlib/mathlib.h>
#include <lib/matrix/matrix/math.hpp>
#include <px4_platform_common/time.h>
#include <math.h>

#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionMultiArray.hpp>
#include <uORB/topics/vehicle_status.h>

#if defined(CONFIG_MAVLINK_ACTUATOR_OUTPUT_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/ACTUATOR_OUTPUT_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_ALTITUDE) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/ALTITUDE.hpp"
#endif
#if defined(CONFIG_MAVLINK_ATTITUDE) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/ATTITUDE.hpp"
#endif
#if defined(CONFIG_MAVLINK_ATTITUDE_QUATERNION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/ATTITUDE_QUATERNION.hpp"
#endif
#if defined(CONFIG_MAVLINK_ATTITUDE_TARGET) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/ATTITUDE_TARGET.hpp"
#endif
#if defined(CONFIG_MAVLINK_AUTOPILOT_VERSION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/AUTOPILOT_VERSION.hpp"
#endif
#if defined(CONFIG_MAVLINK_BATTERY_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/BATTERY_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_CAMERA_IMAGE_CAPTURED) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/CAMERA_IMAGE_CAPTURED.hpp"
#endif
#if defined(CONFIG_MAVLINK_CAMERA_TRIGGER) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/CAMERA_TRIGGER.hpp"
#endif
#if defined(CONFIG_MAVLINK_COLLISION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/COLLISION.hpp"
#endif
#if defined(CONFIG_MAVLINK_COMMAND_LONG) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/COMMAND_LONG.hpp"
#endif
#if defined(CONFIG_MAVLINK_COMPONENT_INFORMATION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/COMPONENT_INFORMATION.hpp"
#endif
#if defined(CONFIG_MAVLINK_COMPONENT_METADATA) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/COMPONENT_METADATA.hpp"
#endif
#if defined(CONFIG_MAVLINK_DISTANCE_SENSOR) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/DISTANCE_SENSOR.hpp"
#endif
#if defined(CONFIG_MAVLINK_EFI_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/EFI_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_ESC_INFO) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/ESC_INFO.hpp"
#endif
#if defined(CONFIG_MAVLINK_ESC_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/ESC_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_ESTIMATOR_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/ESTIMATOR_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_EXTENDED_SYS_STATE) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/EXTENDED_SYS_STATE.hpp"
#endif
#if defined(CONFIG_MAVLINK_FLIGHT_INFORMATION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/FLIGHT_INFORMATION.hpp"
#endif
#if defined(CONFIG_MAVLINK_GLOBAL_POSITION_INT) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/GLOBAL_POSITION_INT.hpp"
#endif
#if defined(CONFIG_MAVLINK_GPS_GLOBAL_ORIGIN) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/GPS_GLOBAL_ORIGIN.hpp"
#endif
#if defined(CONFIG_MAVLINK_GPS_RAW_INT) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/GPS_RAW_INT.hpp"
#endif
#if defined(CONFIG_MAVLINK_GPS_RTCM_DATA) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/GPS_RTCM_DATA.hpp"
#endif
#if defined(CONFIG_MAVLINK_GPS_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/GPS_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_HEARTBEAT) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/HEARTBEAT.hpp"
#endif
#if defined(CONFIG_MAVLINK_HIGHRES_IMU) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/HIGHRES_IMU.hpp"
#endif
#if defined(CONFIG_MAVLINK_HIL_ACTUATOR_CONTROLS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/HIL_ACTUATOR_CONTROLS.hpp"
#endif
#if defined(CONFIG_MAVLINK_HIL_STATE_QUATERNION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/HIL_STATE_QUATERNION.hpp"
#endif
#if defined(CONFIG_MAVLINK_HOME_POSITION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/HOME_POSITION.hpp"
#endif
#if defined(CONFIG_MAVLINK_HYGROMETER_SENSOR) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/HYGROMETER_SENSOR.hpp"
#endif
#if defined(CONFIG_MAVLINK_LANDING_TARGET) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/LANDING_TARGET.hpp"
#endif
#if defined(CONFIG_MAVLINK_LOCAL_POSITION_NED) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/LOCAL_POSITION_NED.hpp"
#endif
#if defined(CONFIG_MAVLINK_MAG_CAL_REPORT) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/MAG_CAL_REPORT.hpp"
#endif
#if defined(CONFIG_MAVLINK_MANUAL_CONTROL) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/MANUAL_CONTROL.hpp"
#endif
#if defined(CONFIG_MAVLINK_MOUNT_ORIENTATION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/MOUNT_ORIENTATION.hpp"
#endif
#if defined(CONFIG_MAVLINK_NAV_CONTROLLER_OUTPUT) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/NAV_CONTROLLER_OUTPUT.hpp"
#endif
#if defined(CONFIG_MAVLINK_OBSTACLE_DISTANCE) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/OBSTACLE_DISTANCE.hpp"
#endif
#if defined(CONFIG_MAVLINK_OPEN_DRONE_ID_BASIC_ID) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/OPEN_DRONE_ID_BASIC_ID.hpp"
#endif
#if defined(CONFIG_MAVLINK_OPEN_DRONE_ID_LOCATION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/OPEN_DRONE_ID_LOCATION.hpp"
#endif
#if defined(CONFIG_MAVLINK_OPEN_DRONE_ID_SYSTEM) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/OPEN_DRONE_ID_SYSTEM.hpp"
#endif
#if defined(CONFIG_MAVLINK_OPTICAL_FLOW_RAD) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/OPTICAL_FLOW_RAD.hpp"
#endif
#if defined(CONFIG_MAVLINK_ORBIT_EXECUTION_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/ORBIT_EXECUTION_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_PING) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/PING.hpp"
#endif
#if defined(CONFIG_MAVLINK_POSITION_TARGET_GLOBAL_INT) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/POSITION_TARGET_GLOBAL_INT.hpp"
#endif
#if defined(CONFIG_MAVLINK_POSITION_TARGET_LOCAL_NED) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/POSITION_TARGET_LOCAL_NED.hpp"
#endif
#if defined(CONFIG_MAVLINK_PROTOCOL_VERSION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/PROTOCOL_VERSION.hpp"
#endif
#if defined(CONFIG_MAVLINK_RAW_RPM) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/RAW_RPM.hpp"
#endif
#if defined(CONFIG_MAVLINK_RC_CHANNELS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/RC_CHANNELS.hpp"
#endif
#if defined(CONFIG_MAVLINK_SCALED_IMU) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/SCALED_IMU.hpp"
#endif
#if defined(CONFIG_MAVLINK_SCALED_IMU2) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/SCALED_IMU2.hpp"
#endif
#if defined(CONFIG_MAVLINK_SCALED_IMU3) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/SCALED_IMU3.hpp"
#endif
#if defined(CONFIG_MAVLINK_SCALED_PRESSURE) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/SCALED_PRESSURE.hpp"
#endif
#if defined(CONFIG_MAVLINK_SERVO_OUTPUT_RAW) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/SERVO_OUTPUT_RAW.hpp"
#endif
#if defined(CONFIG_MAVLINK_STATUSTEXT) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/STATUSTEXT.hpp"
#endif
#if defined(CONFIG_MAVLINK_STORAGE_INFORMATION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/STORAGE_INFORMATION.hpp"
#endif
#if defined(CONFIG_MAVLINK_SYS_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/SYS_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_SYSTEM_TIME) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/SYSTEM_TIME.hpp"
#endif
#if defined(CONFIG_MAVLINK_TIME_ESTIMATE_TO_TARGET) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/TIME_ESTIMATE_TO_TARGET.hpp"
#endif
#if defined(CONFIG_MAVLINK_TIMESYNC) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/TIMESYNC.hpp"
#endif
#if defined(CONFIG_MAVLINK_TRAJECTORY_REPRESENTATION_WAYPOINTS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/TRAJECTORY_REPRESENTATION_WAYPOINTS.hpp"
#endif
#if defined(CONFIG_MAVLINK_VFR_HUD) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/VFR_HUD.hpp"
#endif
#if defined(CONFIG_MAVLINK_VIBRATION) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/VIBRATION.hpp"
#endif
#if defined(CONFIG_MAVLINK_WIND_COV) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/WIND_COV.hpp"
#endif
#if defined(CONFIG_MAVLINK_FIGURE_EIGHT_EXECUTION_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/FIGURE_EIGHT_EXECUTION_STATUS.hpp"
#endif

#ifdef MAVLINK_MSG_ID_AVAILABLE_MODES // Only defined if development.xml is used
#if defined(CONFIG_MAVLINK_AVAILABLE_MODES) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/AVAILABLE_MODES.hpp"
#endif
#if defined(CONFIG_MAVLINK_CURRENT_MODE) || !defined(CONFIG_MAVLINK_MINIMAL)
#include "streams/CURRENT_MODE.hpp"
#endif
#endif

#if !defined(CONSTRAINED_FLASH)
#if defined(CONFIG_MAVLINK_ADSB_VEHICLE) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/ADSB_VEHICLE.hpp"
#endif
#if defined(CONFIG_MAVLINK_AUTOPILOT_STATE_FOR_GIMBAL_DEVICE) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/AUTOPILOT_STATE_FOR_GIMBAL_DEVICE.hpp"
#endif
#if defined(CONFIG_MAVLINK_BATTERY_INFO) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/BATTERY_INFO.hpp"
#endif
#if defined(CONFIG_MAVLINK_DEBUG) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/DEBUG.hpp"
#endif
#if defined(CONFIG_MAVLINK_DEBUG_FLOAT_ARRAY) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/DEBUG_FLOAT_ARRAY.hpp"
#endif
#if defined(CONFIG_MAVLINK_DEBUG_VECT) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/DEBUG_VECT.hpp"
#endif
#if defined(CONFIG_MAVLINK_GIMBAL_DEVICE_ATTITUDE_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/GIMBAL_DEVICE_ATTITUDE_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_GIMBAL_DEVICE_SET_ATTITUDE) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/GIMBAL_DEVICE_SET_ATTITUDE.hpp"
#endif
#if defined(CONFIG_MAVLINK_GIMBAL_MANAGER_INFORMATION) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/GIMBAL_MANAGER_INFORMATION.hpp"
#endif
#if defined(CONFIG_MAVLINK_GIMBAL_MANAGER_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/GIMBAL_MANAGER_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_GIMBAL_DEVICE_INFORMATION) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/GIMBAL_DEVICE_INFORMATION.hpp"
#endif
#if defined(CONFIG_MAVLINK_GPS2_RAW) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/GPS2_RAW.hpp"
#endif
#if defined(CONFIG_MAVLINK_HIGH_LATENCY2) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/HIGH_LATENCY2.hpp"
#endif
#if defined(CONFIG_MAVLINK_LINK_NODE_STATUS) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/LINK_NODE_STATUS.hpp"
#endif
#if defined(CONFIG_MAVLINK_NAMED_VALUE_FLOAT) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/NAMED_VALUE_FLOAT.hpp"
#endif
#if defined(CONFIG_MAVLINK_ODOMETRY) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/ODOMETRY.hpp"
#endif
#if defined(CONFIG_MAVLINK_SCALED_PRESSURE2) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/SCALED_PRESSURE2.hpp"
#endif
#if defined(CONFIG_MAVLINK_SCALED_PRESSURE3) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/SCALED_PRESSURE3.hpp"
#endif
#if defined(CONFIG_MAVLINK_UAVIONIX_ADSB_OUT_CFG) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/UAVIONIX_ADSB_OUT_CFG.hpp"
#endif
#if defined(CONFIG_MAVLINK_UAVIONIX_ADSB_OUT_DYNAMIC) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/UAVIONIX_ADSB_OUT_DYNAMIC.hpp"
#endif
#if defined(CONFIG_MAVLINK_UTM_GLOBAL_POSITION) || !defined(CONFIG_MAVLINK_MINIMAL)
# include "streams/UTM_GLOBAL_POSITION.hpp"
#endif
#endif // !CONSTRAINED_FLASH

// ensure PX4 rotation enum and MAV_SENSOR_ROTATION align
static_assert(MAV_SENSOR_ROTATION_NONE == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_NONE),
	      "Roll: 0, Pitch: 0, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_YAW_45 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_YAW_45),
	      "Roll: 0, Pitch: 0, Yaw: 45");
static_assert(MAV_SENSOR_ROTATION_YAW_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_YAW_90),
	      "Roll: 0, Pitch: 0, Yaw: 90");
static_assert(MAV_SENSOR_ROTATION_YAW_135 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_YAW_135),
	      "Roll: 0, Pitch: 0, Yaw: 135");
static_assert(MAV_SENSOR_ROTATION_YAW_180 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_YAW_180),
	      "Roll: 0, Pitch: 0, Yaw: 180");
static_assert(MAV_SENSOR_ROTATION_YAW_225 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_YAW_225),
	      "Roll: 0, Pitch: 0, Yaw: 225");
static_assert(MAV_SENSOR_ROTATION_YAW_270 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_YAW_270),
	      "Roll: 0, Pitch: 0, Yaw: 270");
static_assert(MAV_SENSOR_ROTATION_YAW_315 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_YAW_315),
	      "Roll: 0, Pitch: 0, Yaw: 315");
static_assert(MAV_SENSOR_ROTATION_ROLL_180 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_180),
	      "Roll: 180, Pitch: 0, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_180_YAW_45 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_180_YAW_45),
	      "Roll: 180, Pitch: 0, Yaw: 45");
static_assert(MAV_SENSOR_ROTATION_ROLL_180_YAW_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_180_YAW_90),
	      "Roll: 180, Pitch: 0, Yaw: 90");
static_assert(MAV_SENSOR_ROTATION_ROLL_180_YAW_135 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_180_YAW_135),
	      "Roll: 180, Pitch: 0, Yaw: 135");
static_assert(MAV_SENSOR_ROTATION_PITCH_180 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_PITCH_180),
	      "Roll: 0, Pitch: 180, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_180_YAW_225 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_180_YAW_225),
	      "Roll: 180, Pitch: 0, Yaw: 225");
static_assert(MAV_SENSOR_ROTATION_ROLL_180_YAW_270 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_180_YAW_270),
	      "Roll: 180, Pitch: 0, Yaw: 270");
static_assert(MAV_SENSOR_ROTATION_ROLL_180_YAW_315 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_180_YAW_315),
	      "Roll: 180, Pitch: 0, Yaw: 315");
static_assert(MAV_SENSOR_ROTATION_ROLL_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_90),
	      "Roll: 90, Pitch: 0, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_YAW_45 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_90_YAW_45),
	      "Roll: 90, Pitch: 0, Yaw: 45");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_YAW_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_90_YAW_90),
	      "Roll: 90, Pitch: 0, Yaw: 90");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_YAW_135 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_90_YAW_135),
	      "Roll: 90, Pitch: 0, Yaw: 135");
static_assert(MAV_SENSOR_ROTATION_ROLL_270 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_270),
	      "Roll: 270, Pitch: 0, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_270_YAW_45 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_270_YAW_45),
	      "Roll: 270, Pitch: 0, Yaw: 45");
static_assert(MAV_SENSOR_ROTATION_ROLL_270_YAW_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_270_YAW_90),
	      "Roll: 270, Pitch: 0, Yaw: 90");
static_assert(MAV_SENSOR_ROTATION_ROLL_270_YAW_135 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_270_YAW_135),
	      "Roll: 270, Pitch: 0, Yaw: 135");
static_assert(MAV_SENSOR_ROTATION_PITCH_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_PITCH_90),
	      "Roll: 0, Pitch: 90, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_PITCH_270 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_PITCH_270),
	      "Roll: 0, Pitch: 270, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_PITCH_180_YAW_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_PITCH_180_YAW_90),
	      "Roll: 0, Pitch: 180, Yaw: 90");
static_assert(MAV_SENSOR_ROTATION_PITCH_180_YAW_270 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_PITCH_180_YAW_270),
	      "Roll: 0, Pitch: 180, Yaw: 270");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_PITCH_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_90_PITCH_90),
	      "Roll: 90, Pitch: 90, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_180_PITCH_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_180_PITCH_90),
	      "Roll: 180, Pitch: 90, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_270_PITCH_90 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_270_PITCH_90),
	      "Roll: 270, Pitch: 90, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_PITCH_180 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_90_PITCH_180),
	      "Roll: 90, Pitch: 180, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_270_PITCH_180 == static_cast<MAV_SENSOR_ORIENTATION>
	      (ROTATION_ROLL_270_PITCH_180), "Roll: 270, Pitch: 180, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_PITCH_270 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_90_PITCH_270),
	      "Roll: 90, Pitch: 270, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_180_PITCH_270 == static_cast<MAV_SENSOR_ORIENTATION>
	      (ROTATION_ROLL_180_PITCH_270), "Roll: 180, Pitch: 270, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_270_PITCH_270 == static_cast<MAV_SENSOR_ORIENTATION>
	      (ROTATION_ROLL_270_PITCH_270), "Roll: 270, Pitch: 270, Yaw: 0");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_PITCH_180_YAW_90 == static_cast<MAV_SENSOR_ORIENTATION>
	      (ROTATION_ROLL_90_PITCH_180_YAW_90),
	      "Roll: 90, Pitch: 180, Yaw: 90");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_YAW_270 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_90_YAW_270),
	      "Roll: 90, Pitch: 0, Yaw: 270");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_PITCH_68_YAW_293 == static_cast<MAV_SENSOR_ORIENTATION>
	      (ROTATION_ROLL_90_PITCH_68_YAW_293),
	      "Roll: 90, Pitch: 68, Yaw: 293");
static_assert(MAV_SENSOR_ROTATION_PITCH_315 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_PITCH_315), "Pitch: 315");
static_assert(MAV_SENSOR_ROTATION_ROLL_90_PITCH_315 == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_ROLL_90_PITCH_315),
	      "Roll: 90, Pitch: 315");

// Note: Update the number (41, as of writing) below to the number of 'normal' rotation enums in MAVLink spec:
// https://mavlink.io/en/messages/common.html#MAV_SENSOR_ORIENTATION
static_assert(41 == ROTATION_MAX, "Keep MAV_SENSOR_ROTATION and PX4 Rotation in sync");

static_assert(MAV_SENSOR_ROTATION_CUSTOM == static_cast<MAV_SENSOR_ORIENTATION>(ROTATION_CUSTOM), "Custom Rotation");


static const StreamListItem streams_list[] = {
#if defined(HEARTBEAT_HPP)
	create_stream_list_item<MavlinkStreamHeartbeat>(),
#endif // HEARTBEAT_HPP
#if defined(STATUSTEXT_HPP)
	create_stream_list_item<MavlinkStreamStatustext>(),
#endif // STATUSTEXT_HPP
#if defined(COMMAND_LONG_HPP)
	create_stream_list_item<MavlinkStreamCommandLong>(),
#endif // COMMAND_LONG_HPP
#if defined(SYS_STATUS_HPP)
	create_stream_list_item<MavlinkStreamSysStatus>(),
#endif // SYS_STATUS_HPP
#if defined(BATTERY_STATUS)
	create_stream_list_item<MavlinkStreamBatteryStatus>(),
#endif // BATTERY_STATUS
#if defined(BATTERY_INFO_HPP)
	create_stream_list_item<MavlinkStreamBatteryInfo>(),
#endif // BATTERY_INFO_HPP
#if defined(HIGHRES_IMU_HPP)
	create_stream_list_item<MavlinkStreamHighresIMU>(),
#endif // HIGHRES_IMU_HPP
#if defined(SCALED_IMU_HPP)
	create_stream_list_item<MavlinkStreamScaledIMU>(),
#endif // SCALED_IMU_HPP
#if defined(SCALED_IMU2_HPP)
	create_stream_list_item<MavlinkStreamScaledIMU2>(),
#endif // SCALED_IMU2_HPP
#if defined(SCALED_IMU3_HPP)
	create_stream_list_item<MavlinkStreamScaledIMU3>(),
#endif // SCALED_IMU3_HPP
#if defined(SCALED_PRESSURE_HPP)
	create_stream_list_item<MavlinkStreamScaledPressure>(),
#endif // SCALED_PRESSURE
#if defined(SCALED_PRESSURE2_HPP)
	create_stream_list_item<MavlinkStreamScaledPressure2>(),
#endif // SCALED_PRESSURE2
#if defined(SCALED_PRESSURE3_HPP)
	create_stream_list_item<MavlinkStreamScaledPressure3>(),
#endif // SCALED_PRESSURE3
#if defined(ACTUATOR_OUTPUT_STATUS_HPP)
	create_stream_list_item<MavlinkStreamActuatorOutputStatus>(),
#endif // ACTUATOR_OUTPUT_STATUS_HPP
#if defined(ATTITUDE_HPP)
	create_stream_list_item<MavlinkStreamAttitude>(),
#endif // ATTITUDE_HPP
#if defined(ATTITUDE_QUATERNION_HPP)
	create_stream_list_item<MavlinkStreamAttitudeQuaternion>(),
#endif // ATTITUDE_QUATERNION_HPP
#if defined(VFR_HUD_HPP)
	create_stream_list_item<MavlinkStreamVFRHUD>(),
#endif // VFR_HUD_HPP
#if defined(GPS_GLOBAL_ORIGIN_HPP)
	create_stream_list_item<MavlinkStreamGpsGlobalOrigin>(),
#endif // GPS_GLOBAL_ORIGIN_HPP
#if defined(GPS_RAW_INT_HPP)
	create_stream_list_item<MavlinkStreamGPSRawInt>(),
#endif // GPS_RAW_INT_HPP
#if defined(GPS2_RAW_HPP)
	create_stream_list_item<MavlinkStreamGPS2Raw>(),
#endif // GPS2_RAW_HPP
#if defined(SYSTEM_TIME_HPP)
	create_stream_list_item<MavlinkStreamSystemTime>(),
#endif // SYSTEM_TIME_HPP
#if defined(TIME_ESTIMATE_TO_TARGET_HPP)
	create_stream_list_item<MavlinkStreamTimeEstimateToTarget>(),
#endif // TIME_ESTIMATE_TO_TARGET_HPP
#if defined(TIMESYNC_HPP)
	create_stream_list_item<MavlinkStreamTimesync>(),
#endif // TIMESYNC_HPP
#if defined(GLOBAL_POSITION_INT_HPP)
	create_stream_list_item<MavlinkStreamGlobalPositionInt>(),
#endif // GLOBAL_POSITION_INT_HPP
#if defined(LANDING_TARGET_HPP)
	create_stream_list_item<MavlinkStreamLandingTarget>(),
#endif
#if defined(LOCAL_POSITION_NED_HPP)
	create_stream_list_item<MavlinkStreamLocalPositionNED>(),
#endif // LOCAL_POSITION_NED_HPP
#if defined(MAG_CAL_REPORT_HPP)
	create_stream_list_item<MavlinkStreamMagCalReport>(),
#endif // MAG_CAL_REPORT_HPP
#if defined(ODOMETRY_HPP)
	create_stream_list_item<MavlinkStreamOdometry>(),
#endif // ODOMETRY_HPP
#if defined(ESTIMATOR_STATUS_HPP)
	create_stream_list_item<MavlinkStreamEstimatorStatus>(),
#endif // ESTIMATOR_STATUS_HPP
#if defined(VIBRATION_HPP)
	create_stream_list_item<MavlinkStreamVibration>(),
#endif // VIBRATION_HPP
#if defined(AUTOPILOT_STATE_FOR_GIMBAL_DEVICE_HPP)
	create_stream_list_item<MavlinkStreamAutopilotStateForGimbalDevice>(),
#endif // AUTOPILOT_STATE_FOR_GIMBAL_DEVICE_HPP
#if defined(GIMBAL_DEVICE_ATTITUDE_STATUS_HPP)
	create_stream_list_item<MavlinkStreamGimbalDeviceAttitudeStatus>(),
#endif // GIMBAL_DEVICE_ATTITUDE_STATUS_HPP
#if defined(GIMBAL_MANAGER_INFORMATION_HPP)
	create_stream_list_item<MavlinkStreamGimbalManagerInformation>(),
#endif // GIMBAL_MANAGER_INFORMATION_HPP
#if defined(GIMBAL_DEVICE_INFORMATION_HPP)
	create_stream_list_item<MavlinkStreamGimbalDeviceInformation>(),
#endif // GIMBAL_DEVICE_INFORMATION_HPP
#if defined(GIMBAL_MANAGER_STATUS_HPP)
	create_stream_list_item<MavlinkStreamGimbalManagerStatus>(),
#endif // GIMBAL_MANAGER_STATUS_HPP
#if defined(GIMBAL_DEVICE_SET_ATTITUDE_HPP)
	create_stream_list_item<MavlinkStreamGimbalDeviceSetAttitude>(),
#endif // GIMBAL_DEVICE_SET_ATTITUDE_HPP
#if defined(HOME_POSITION_HPP)
	create_stream_list_item<MavlinkStreamHomePosition>(),
#endif // HOME_POSITION_HPP
#if defined(HYGROMETER_SENSOR_HPP)
	create_stream_list_item<MavlinkStreamHygrometerSensor>(),
#endif // HYGROMETER_SENSOR_HPP
#if defined(SERVO_OUTPUT_RAW_HPP)
	create_stream_list_item<MavlinkStreamServoOutputRaw<0> >(),
	create_stream_list_item<MavlinkStreamServoOutputRaw<1> >(),
#endif // SERVO_OUTPUT_RAW_HPP
#if defined(HIL_ACTUATOR_CONTROLS_HPP)
	create_stream_list_item<MavlinkStreamHILActuatorControls>(),
#endif // HIL_ACTUATOR_CONTROLS_HPP
#if defined(POSITION_TARGET_GLOBAL_INT_HPP)
	create_stream_list_item<MavlinkStreamPositionTargetGlobalInt>(),
#endif // POSITION_TARGET_GLOBAL_INT_HPP
#if defined(POSITION_TARGET_LOCAL_NED_HPP)
	create_stream_list_item<MavlinkStreamPositionTargetLocalNed>(),
#endif // POSITION_TARGET_LOCAL_NED_HPP
#if defined(ATTITUDE_TARGET_HPP)
	create_stream_list_item<MavlinkStreamAttitudeTarget>(),
#endif // ATTITUDE_TARGET_HPP
#if defined(RC_CHANNELS_HPP)
	create_stream_list_item<MavlinkStreamRCChannels>(),
#endif // RC_CHANNELS_HPP
#if defined(MANUAL_CONTROL_HPP)
	create_stream_list_item<MavlinkStreamManualControl>(),
#endif // MANUAL_CONTROL_HPP
#if defined(TRAJECTORY_REPRESENTATION_WAYPOINTS_HPP)
	create_stream_list_item<MavlinkStreamTrajectoryRepresentationWaypoints>(),
#endif // TRAJECTORY_REPRESENTATION_WAYPOINTS_HPP
#if defined(OPTICAL_FLOW_RAD_HPP)
	create_stream_list_item<MavlinkStreamOpticalFlowRad>(),
#endif // OPTICAL_FLOW_RAD_HPP
#if defined(NAMED_VALUE_FLOAT_HPP)
	create_stream_list_item<MavlinkStreamNamedValueFloat>(),
#endif // NAMED_VALUE_FLOAT_HPP
#if defined(DEBUG_HPP)
	create_stream_list_item<MavlinkStreamDebug>(),
#endif // DEBUG_HPP
#if defined(DEBUG_VECT_HPP)
	create_stream_list_item<MavlinkStreamDebugVect>(),
#endif // DEBUG_VECT_HPP
#if defined(DEBUG_FLOAT_ARRAY_HPP)
	create_stream_list_item<MavlinkStreamDebugFloatArray>(),
#endif // DEBUG_FLOAT_ARRAY_HPP
#if defined(NAV_CONTROLLER_OUTPUT_HPP)
	create_stream_list_item<MavlinkStreamNavControllerOutput>(),
#endif // NAV_CONTROLLER_OUTPUT_HPP
#if defined(CAMERA_TRIGGER_HPP)
	create_stream_list_item<MavlinkStreamCameraTrigger>(),
#endif // CAMERA_TRIGGER_HPP
#if defined(CAMERA_IMAGE_CAPTURED_HPP)
	create_stream_list_item<MavlinkStreamCameraImageCaptured>(),
#endif // CAMERA_IMAGE_CAPTURED_HPP
#if defined(DISTANCE_SENSOR_HPP)
	create_stream_list_item<MavlinkStreamDistanceSensor>(),
#endif // DISTANCE_SENSOR_HPP
#if defined(EXTENDED_SYS_STATE_HPP)
	create_stream_list_item<MavlinkStreamExtendedSysState>(),
#endif // EXTENDED_SYS_STATE_HPP
#if defined(ALTITUDE_HPP)
	create_stream_list_item<MavlinkStreamAltitude>(),
#endif // ALTITUDE_HPP
#if defined(ADSB_VEHICLE_HPP)
	create_stream_list_item<MavlinkStreamADSBVehicle>(),
#endif // ADSB_VEHICLE_HPP
#if defined(UTM_GLOBAL_POSITION_HPP)
	create_stream_list_item<MavlinkStreamUTMGlobalPosition>(),
#endif // UTM_GLOBAL_POSITION_HPP
#if defined(COLLISION_HPP)
	create_stream_list_item<MavlinkStreamCollision>(),
#endif // COLLISION_HPP
#if defined(WIND_COV_HPP)
	create_stream_list_item<MavlinkStreamWindCov>(),
#endif // WIND_COV_HPP
#if defined(MOUNT_ORIENTATION_HPP)
	create_stream_list_item<MavlinkStreamMountOrientation>(),
#endif // MOUNT_ORIENTATION_HPP
#if defined(HIGH_LATENCY2_HPP)
	create_stream_list_item<MavlinkStreamHighLatency2>(),
#endif // HIGH_LATENCY2_HPP
#if defined(HIL_STATE_QUATERNION_HPP)
	create_stream_list_item<MavlinkStreamHILStateQuaternion>(),
#endif // HIL_STATE_QUATERNION_HPP
#if defined(PING_HPP)
	create_stream_list_item<MavlinkStreamPing>(),
#endif // PING_HPP
#if defined(ORBIT_EXECUTION_STATUS_HPP)
	create_stream_list_item<MavlinkStreamOrbitStatus>(),
#endif // ORBIT_EXECUTION_STATUS_HPP
#if defined(FIGURE_EIGHT_EXECUTION_STATUS_HPP)
	create_stream_list_item<MavlinkStreamFigureEightStatus>(),
#endif // FIGURE_EIGHT_EXECUTION_STATUS_HPP
#if defined(OBSTACLE_DISTANCE_HPP)
	create_stream_list_item<MavlinkStreamObstacleDistance>(),
#endif // OBSTACLE_DISTANCE_HPP
#if defined(OPEN_DRONE_ID_BASIC_ID_HPP)
	create_stream_list_item<MavlinkStreamOpenDroneIdBasicId>(),
#endif // OPEN_DRONE_ID_BASIC_ID_HPP
#if defined(OPEN_DRONE_ID_LOCATION_HPP)
	create_stream_list_item<MavlinkStreamOpenDroneIdLocation>(),
#endif // OPEN_DRONE_ID_LOCATION_HPP
#if defined(OPEN_DRONE_ID_SYSTEM_HPP)
	create_stream_list_item<MavlinkStreamOpenDroneIdSystem>(),
#endif // OPEN_DRONE_ID_SYSTEM_HPP
#if defined(ESC_INFO_HPP)
	create_stream_list_item<MavlinkStreamESCInfo>(),
#endif // ESC_INFO_HPP
#if defined(ESC_STATUS_HPP)
	create_stream_list_item<MavlinkStreamESCStatus>(),
#endif // ESC_STATUS_HPP
#if defined(AUTOPILOT_VERSION_HPP)
	create_stream_list_item<MavlinkStreamAutopilotVersion>(),
#endif // AUTOPILOT_VERSION_HPP
#if defined(PROTOCOL_VERSION_HPP)
	create_stream_list_item<MavlinkStreamProtocolVersion>(),
#endif // PROTOCOL_VERSION_HPP
#if defined(FLIGHT_INFORMATION_HPP)
	create_stream_list_item<MavlinkStreamFlightInformation>(),
#endif // FLIGHT_INFORMATION_HPP
#if defined(GPS_STATUS_HPP)
	create_stream_list_item<MavlinkStreamGPSStatus>(),
#endif // GPS_STATUS_HPP
#if defined(LINK_NODE_STATUS_HPP)
	create_stream_list_item<MavlinkStreamLinkNodeStatus>(),
#endif // LINK_NODE_STATUS_HPP
#if defined(STORAGE_INFORMATION_HPP)
	create_stream_list_item<MavlinkStreamStorageInformation>(),
#endif // STORAGE_INFORMATION_HPP
#if defined(COMPONENT_INFORMATION_HPP)
	create_stream_list_item<MavlinkStreamComponentInformation>(),
#endif // COMPONENT_INFORMATION_HPP
#if defined(COMPONENT_METADATA_HPP)
	create_stream_list_item<MavlinkStreamComponentMetadata>(),
#endif // COMPONENT_METADATA_HPP
#if defined(RAW_RPM_HPP)
	create_stream_list_item<MavlinkStreamRawRpm>(),
#endif // RAW_RPM_HPP
#if defined(EFI_STATUS_HPP)
	create_stream_list_item<MavlinkStreamEfiStatus>(),
#endif // EFI_STATUS_HPP
#if defined(GPS_RTCM_DATA_HPP)
	create_stream_list_item<MavlinkStreamGPSRTCMData>(),
#endif // GPS_RTCM_DATA_HPP
#if defined(UAVIONIX_ADSB_OUT_CFG_HPP)
	create_stream_list_item<MavlinkStreamUavionixADSBOutCfg>(),
#endif // UAVIONIX_ADSB_OUT_CFG_HPP
#if defined(UAVIONIX_ADSB_OUT_DYNAMIC_HPP)
	create_stream_list_item<MavlinkStreamUavionixADSBOutDynamic>(),
#endif // UAVIONIX_ADSB_OUT_DYNAMIC_HPP
#if defined(AVAILABLE_MODES_HPP)
	create_stream_list_item<MavlinkStreamAvailableModes>(),
#endif // AVAILABLE_MODES_HPP
#if defined(CURRENT_MODE_HPP)
	create_stream_list_item<MavlinkStreamCurrentMode>(),
#endif // CURRENT_MODE_HPP
};

const char *get_stream_name(const uint16_t msg_id)
{
	// search for stream with specified msg id in supported streams list
	for (const auto &stream : streams_list) {
		if (msg_id == stream.get_id()) {
			return stream.get_name();
		}
	}

	return nullptr;
}

MavlinkStream *create_mavlink_stream(const char *stream_name, Mavlink *mavlink)
{
	// search for stream with specified name in supported streams list
	if (stream_name != nullptr) {
		for (const auto &stream : streams_list) {
			if (strcmp(stream_name, stream.get_name()) == 0) {
				return stream.new_instance(mavlink);
			}
		}
	}

	return nullptr;
}

MavlinkStream *create_mavlink_stream(const uint16_t msg_id, Mavlink *mavlink)
{
	// search for stream with specified name in supported streams list
	for (const auto &stream : streams_list) {
		if (msg_id == stream.get_id()) {
			return stream.new_instance(mavlink);
		}
	}

	return nullptr;
}
