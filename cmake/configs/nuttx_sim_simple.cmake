include(nuttx/px4_impl_nuttx)

message(WARNING "this is a work in progress and doesn't build yet")

set(CMAKE_TOOLCHAIN_FILE cmake/toolchains/Toolchain-native.cmake)

set(config_module_list
	#platforms/nuttx
	#platforms/nuttx/px4_layer
	platforms/common
	#drivers/led
	drivers/device
	#modules/systemlib
	#modules/uORB
	#examples/px4_simple_app
	#lib/mathlib/math/filter
	#lib/conversion
	)

set(config_firmware_options
	PARAM_XML # generate param xml
	)

set(config_extra_builtin_cmds
	)
