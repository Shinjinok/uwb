px4_add_board(
PLATFORM nuttx
VENDOR myboard
MODEL myfc
LABEL default
TOOLCHAIN arm-none-eabi
ARCHITECTURE cortex-m0plus
CONSTRAINED_MEMORY
ROMFSROOT px4fmu_common
CONSTRAINED_FLASH
SERIAL_PORTS
	TEL1:/dev/ttyS1
	GPS1:/dev/ttyS2
DRIVERS
	adc/board_adc
# 	#barometer # all available barometer drivers
	barometer/bmp280
# 	#batt_smbus
# 	#camera_trigger
# 	#differential_pressure # all available differential pressure drivers
# 	#distance_sensor # all available distance sensor drivers
# 	dshot
	gps
# 	imu/invensense/icm20602
	imu/invensense/mpu9250
# 	#irlock
# 	lights/rgbled
# 	#magnetometer # all available magnetometer drivers
	magnetometer/hmc5883
# 	#optical_flow # all available optical flow drivers
# 	osd
# 	#pca9685
# 	#pwm_input
# 	#pwm_out_sim
# 	pwm_out
	rc_input
# 	#telemetry # all available telemetry drivers
# 	telemetry/frsky_telemetry
MODULES
# 	#airspeed_selector
# 	attitude_estimator_q
# 	battery_status
# 	#camera_feedback
# 	commander
	dataman
# 	ekf2
# 	#esc_battery
# 	events
# 	flight_mode_manager
# 	#fw_att_control
# 	#fw_pos_control_l1
# 	gyro_calibration
# 	#gyro_fft
# 	land_detector
# 	#landing_target_estimator
# 	load_mon
# 	#local_position_estimator
	logger # There was a linker error about "undefined reference to `__aeabi_fcmpun'" when this was activated while led_control, nshterm, reboot, top, uorb, ver, adc/board_adc, gps active and everything else inactive. The error then surprisingly goes away when barometer/bmp280 and imu/invensense/mpu9250 are activated.
	mavlink
# 	mc_att_control
# 	mc_hover_thrust_estimator
# 	mc_pos_control
# 	mc_rate_control
# 	#micrortps_bridge
# 	navigator
	rc_update
# 	#rover_pos_control
	sensors
# 	#sih
# 	#temperature_compensation
# 	#uuv_att_control
# 	#uuv_pos_control
# 	#vmount
# 	#vtol_att_control
SYSTEMCMDS
# 	#bl_update
# 	dmesg
# 	dumpfile
# 	esc_calib
# 	#gpio
# 	hardfault_log
	i2cdetect
	led_control
# 	#mft
# 	mixer
# 	#motor_ramp
# 	motor_test
# 	#mtd
	nshterm
	param
# 	perf
# 	pwm
	reboot
# 	reflect
# 	sd_bench
#	# serial_test
# 	#system_time
	top
# 	#topic_listener
	tune_control
	uorb
	usb_connected
	ver
	work_queue
)
