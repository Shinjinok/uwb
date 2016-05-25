#
# Automatically generated file; DO NOT EDIT.
# Nuttx/ Configuration
#

#
# Build Setup
#
# CONFIG_EXPERIMENTAL is not set
CONFIG_DEFAULT_SMALL=y
CONFIG_HOST_LINUX=y
# CONFIG_HOST_OSX is not set
# CONFIG_HOST_WINDOWS is not set
# CONFIG_HOST_OTHER is not set

#
# Build Configuration
#
CONFIG_APPS_DIR="../apps"
CONFIG_BUILD_FLAT=y
# CONFIG_BUILD_2PASS is not set

#
# Binary Output Formats
#
# CONFIG_RRLOAD_BINARY is not set
# CONFIG_INTELHEX_BINARY is not set
# CONFIG_MOTOROLA_SREC is not set
CONFIG_RAW_BINARY=y
# CONFIG_UBOOT_UIMAGE is not set

#
# Customize Header Files
#
# CONFIG_ARCH_STDINT_H is not set
# CONFIG_ARCH_STDBOOL_H is not set
CONFIG_ARCH_MATH_H=y
# CONFIG_ARCH_FLOAT_H is not set
# CONFIG_ARCH_STDARG_H is not set

#
# Debug Options
#
CONFIG_DEBUG=y
CONFIG_DEBUG_VERBOSE=y
CONFIG_ARCH_HAVE_HEAPCHECK=y
CONFIG_DEBUG_VERBOSE=y

#
# Subsystem Debug Options
#
# CONFIG_DEBUG_AUDIO is not set
# CONFIG_DEBUG_BINFMT is not set
# CONFIG_DEBUG_FS is not set
# CONFIG_DEBUG_GRAPHICS is not set
# CONFIG_DEBUG_LIB is not set
# CONFIG_DEBUG_MM is not set
# CONFIG_DEBUG_SCHED is not set

#
# OS Function Debug Options
#
# CONFIG_DEBUG_DMA is not set
# CONFIG_DEBUG_HEAP is not set
# CONFIG_DEBUG_IRQ is not set

#
# Driver Debug Options
#
# CONFIG_DEBUG_ANALOG is not set
# CONFIG_DEBUG_CAN is not set
# CONFIG_DEBUG_GPIO is not set
# CONFIG_DEBUG_SPI is not set
CONFIG_ARCH_HAVE_STACKCHECK=y
CONFIG_STACK_COLORATION=y
CONFIG_DEBUG_SYMBOLS=y
CONFIG_ARCH_HAVE_CUSTOMOPT=y
# CONFIG_DEBUG_NOOPT is not set
# CONFIG_DEBUG_CUSTOMOPT is not set
CONFIG_DEBUG_FULLOPT=y

#
# System Type
#
CONFIG_ARCH_ARM=y
# CONFIG_ARCH_AVR is not set
# CONFIG_ARCH_HC is not set
# CONFIG_ARCH_MIPS is not set
# CONFIG_ARCH_RGMP is not set
# CONFIG_ARCH_SH is not set
# CONFIG_ARCH_SIM is not set
# CONFIG_ARCH_X86 is not set
# CONFIG_ARCH_Z16 is not set
# CONFIG_ARCH_Z80 is not set
CONFIG_ARCH="arm"

#
# ARM Options
#
# CONFIG_ARCH_CHIP_A1X is not set
# CONFIG_ARCH_CHIP_C5471 is not set
# CONFIG_ARCH_CHIP_CALYPSO is not set
# CONFIG_ARCH_CHIP_DM320 is not set
# CONFIG_ARCH_CHIP_EFM32 is not set
# CONFIG_ARCH_CHIP_IMX is not set
# CONFIG_ARCH_CHIP_KINETIS is not set
# CONFIG_ARCH_CHIP_KL is not set
# CONFIG_ARCH_CHIP_LM is not set
# CONFIG_ARCH_CHIP_TIVA is not set
# CONFIG_ARCH_CHIP_LPC17XX is not set
# CONFIG_ARCH_CHIP_LPC214X is not set
# CONFIG_ARCH_CHIP_LPC2378 is not set
# CONFIG_ARCH_CHIP_LPC31XX is not set
# CONFIG_ARCH_CHIP_LPC43XX is not set
# CONFIG_ARCH_CHIP_NUC1XX is not set
# CONFIG_ARCH_CHIP_SAMA5 is not set
# CONFIG_ARCH_CHIP_SAMD is not set
# CONFIG_ARCH_CHIP_SAM34 is not set
# CONFIG_ARCH_CHIP_SAMV7 is not set
CONFIG_ARCH_CHIP_STM32=y
# CONFIG_ARCH_CHIP_STR71X is not set
# CONFIG_ARCH_ARM7TDMI is not set
# CONFIG_ARCH_ARM926EJS is not set
# CONFIG_ARCH_ARM920T is not set
# CONFIG_ARCH_CORTEXM0 is not set
CONFIG_ARCH_CORTEXM3=y
# CONFIG_ARCH_CORTEXM4 is not set
# CONFIG_ARCH_CORTEXM7 is not set
# CONFIG_ARCH_CORTEXA5 is not set
# CONFIG_ARCH_CORTEXA8 is not set
CONFIG_ARCH_FAMILY="armv7-m"
CONFIG_ARCH_CHIP="stm32"
CONFIG_ARMV7M_USEBASEPRI=y
CONFIG_ARCH_HAVE_CMNVECTOR=y
CONFIG_ARMV7M_CMNVECTOR=y
# CONFIG_ARMV7M_LAZYFPU is not set
# CONFIG_ARCH_HAVE_FPU is not set
# CONFIG_ARCH_HAVE_DPFPU is not set
# CONFIG_ARMV7M_MPU is not set
# CONFIG_DEBUG_HARDFAULT is not set

#
# ARMV7M Configuration Options
#
# CONFIG_ARMV7M_HAVE_ICACHE is not set
# CONFIG_ARMV7M_HAVE_DCACHE is not set
# CONFIG_ARMV7M_HAVE_ITCM is not set
# CONFIG_ARMV7M_HAVE_DTCM is not set
# CONFIG_ARMV7M_TOOLCHAIN_BUILDROOT is not set
# CONFIG_ARMV7M_TOOLCHAIN_CODEREDL is not set
CONFIG_ARMV7M_TOOLCHAIN_CODESOURCERYL=y
# CONFIG_ARMV7M_TOOLCHAIN_GNU_EABIL is not set
# CONFIG_ARMV7M_STACKCHECK is not set
# CONFIG_ARMV7M_ITMSYSLOG is not set

#
# STM32 Configuration Options
#
# CONFIG_ARCH_CHIP_STM32L151C6 is not set
# CONFIG_ARCH_CHIP_STM32L151C8 is not set
# CONFIG_ARCH_CHIP_STM32L151CB is not set
# CONFIG_ARCH_CHIP_STM32L151R6 is not set
# CONFIG_ARCH_CHIP_STM32L151R8 is not set
# CONFIG_ARCH_CHIP_STM32L151RB is not set
# CONFIG_ARCH_CHIP_STM32L151V6 is not set
# CONFIG_ARCH_CHIP_STM32L151V8 is not set
# CONFIG_ARCH_CHIP_STM32L151VB is not set
# CONFIG_ARCH_CHIP_STM32L152C6 is not set
# CONFIG_ARCH_CHIP_STM32L152C8 is not set
# CONFIG_ARCH_CHIP_STM32L152CB is not set
# CONFIG_ARCH_CHIP_STM32L152R6 is not set
# CONFIG_ARCH_CHIP_STM32L152R8 is not set
# CONFIG_ARCH_CHIP_STM32L152RB is not set
# CONFIG_ARCH_CHIP_STM32L152V6 is not set
# CONFIG_ARCH_CHIP_STM32L152V8 is not set
# CONFIG_ARCH_CHIP_STM32L152VB is not set
# CONFIG_ARCH_CHIP_STM32L162ZD is not set
# CONFIG_ARCH_CHIP_STM32L162VE is not set
# CONFIG_ARCH_CHIP_STM32F100C8 is not set
# CONFIG_ARCH_CHIP_STM32F100CB is not set
# CONFIG_ARCH_CHIP_STM32F100R8 is not set
# CONFIG_ARCH_CHIP_STM32F100RB is not set
# CONFIG_ARCH_CHIP_STM32F100RC is not set
# CONFIG_ARCH_CHIP_STM32F100RD is not set
# CONFIG_ARCH_CHIP_STM32F100RE is not set
# CONFIG_ARCH_CHIP_STM32F100V8 is not set
# CONFIG_ARCH_CHIP_STM32F100VB is not set
# CONFIG_ARCH_CHIP_STM32F100VC is not set
# CONFIG_ARCH_CHIP_STM32F100VD is not set
# CONFIG_ARCH_CHIP_STM32F100VE is not set
# CONFIG_ARCH_CHIP_STM32F102CB is not set
# CONFIG_ARCH_CHIP_STM32F103T8 is not set
# CONFIG_ARCH_CHIP_STM32F103TB is not set
# CONFIG_ARCH_CHIP_STM32F103C4 is not set
# CONFIG_ARCH_CHIP_STM32F103C8 is not set
# CONFIG_ARCH_CHIP_STM32F103CB is not set
# CONFIG_ARCH_CHIP_STM32F103R8 is not set
CONFIG_ARCH_CHIP_STM32F103RB=y
# CONFIG_ARCH_CHIP_STM32F103RC is not set
# CONFIG_ARCH_CHIP_STM32F103RD is not set
# CONFIG_ARCH_CHIP_STM32F103RE is not set
# CONFIG_ARCH_CHIP_STM32F103RG is not set
# CONFIG_ARCH_CHIP_STM32F103V8 is not set
# CONFIG_ARCH_CHIP_STM32F103VB is not set
# CONFIG_ARCH_CHIP_STM32F103VC is not set
# CONFIG_ARCH_CHIP_STM32F103VE is not set
# CONFIG_ARCH_CHIP_STM32F103ZE is not set
# CONFIG_ARCH_CHIP_STM32F105VB is not set
# CONFIG_ARCH_CHIP_STM32F107VC is not set
# CONFIG_ARCH_CHIP_STM32F207IG is not set
# CONFIG_ARCH_CHIP_STM32F207ZE is not set
# CONFIG_ARCH_CHIP_STM32F302CB is not set
# CONFIG_ARCH_CHIP_STM32F302CC is not set
# CONFIG_ARCH_CHIP_STM32F302RB is not set
# CONFIG_ARCH_CHIP_STM32F302RC is not set
# CONFIG_ARCH_CHIP_STM32F302VB is not set
# CONFIG_ARCH_CHIP_STM32F302VC is not set
# CONFIG_ARCH_CHIP_STM32F303CB is not set
# CONFIG_ARCH_CHIP_STM32F303CC is not set
# CONFIG_ARCH_CHIP_STM32F303RB is not set
# CONFIG_ARCH_CHIP_STM32F303RC is not set
# CONFIG_ARCH_CHIP_STM32F303VB is not set
# CONFIG_ARCH_CHIP_STM32F303VC is not set
# CONFIG_ARCH_CHIP_STM32F372C8 is not set
# CONFIG_ARCH_CHIP_STM32F372R8 is not set
# CONFIG_ARCH_CHIP_STM32F372V8 is not set
# CONFIG_ARCH_CHIP_STM32F372CB is not set
# CONFIG_ARCH_CHIP_STM32F372RB is not set
# CONFIG_ARCH_CHIP_STM32F372VB is not set
# CONFIG_ARCH_CHIP_STM32F372CC is not set
# CONFIG_ARCH_CHIP_STM32F372RC is not set
# CONFIG_ARCH_CHIP_STM32F372VC is not set
# CONFIG_ARCH_CHIP_STM32F373C8 is not set
# CONFIG_ARCH_CHIP_STM32F373R8 is not set
# CONFIG_ARCH_CHIP_STM32F373V8 is not set
# CONFIG_ARCH_CHIP_STM32F373CB is not set
# CONFIG_ARCH_CHIP_STM32F373RB is not set
# CONFIG_ARCH_CHIP_STM32F373VB is not set
# CONFIG_ARCH_CHIP_STM32F373CC is not set
# CONFIG_ARCH_CHIP_STM32F373RC is not set
# CONFIG_ARCH_CHIP_STM32F373VC is not set
# CONFIG_ARCH_CHIP_STM32F401RE is not set
# CONFIG_ARCH_CHIP_STM32F411RE is not set
# CONFIG_ARCH_CHIP_STM32F405RG is not set
# CONFIG_ARCH_CHIP_STM32F405VG is not set
# CONFIG_ARCH_CHIP_STM32F405ZG is not set
# CONFIG_ARCH_CHIP_STM32F407VE is not set
# CONFIG_ARCH_CHIP_STM32F407VG is not set
# CONFIG_ARCH_CHIP_STM32F407ZE is not set
# CONFIG_ARCH_CHIP_STM32F407ZG is not set
# CONFIG_ARCH_CHIP_STM32F407IE is not set
# CONFIG_ARCH_CHIP_STM32F407IG is not set
# CONFIG_ARCH_CHIP_STM32F427V is not set
# CONFIG_ARCH_CHIP_STM32F427Z is not set
# CONFIG_ARCH_CHIP_STM32F427I is not set
# CONFIG_ARCH_CHIP_STM32F429V is not set
# CONFIG_ARCH_CHIP_STM32F429Z is not set
# CONFIG_ARCH_CHIP_STM32F429I is not set
# CONFIG_ARCH_CHIP_STM32F429B is not set
# CONFIG_ARCH_CHIP_STM32F429N is not set
# CONFIG_STM32_STM32L15XX is not set
# CONFIG_STM32_ENERGYLITE is not set
CONFIG_STM32_STM32F10XX=y
# CONFIG_STM32_VALUELINE is not set
# CONFIG_STM32_CONNECTIVITYLINE is not set
CONFIG_STM32_PERFORMANCELINE=y
# CONFIG_STM32_USBACCESSLINE is not set
# CONFIG_STM32_HIGHDENSITY is not set
CONFIG_STM32_MEDIUMDENSITY=y
# CONFIG_STM32_LOWDENSITY is not set
# CONFIG_STM32_STM32F20XX is not set
# CONFIG_STM32_STM32F207 is not set
# CONFIG_STM32_STM32F30XX is not set
# CONFIG_STM32_STM32F37XX is not set
# CONFIG_STM32_STM32F40XX is not set
# CONFIG_STM32_STM32F401 is not set
# CONFIG_STM32_STM32F411 is not set
# CONFIG_STM32_STM32F405 is not set
# CONFIG_STM32_STM32F407 is not set
# CONFIG_STM32_STM32F427 is not set
# CONFIG_STM32_STM32F429 is not set
# CONFIG_STM32_DFU is not set

#
# STM32 Peripheral Support
#
# CONFIG_STM32_HAVE_CCM is not set
CONFIG_STM32_HAVE_USBDEV=y
# CONFIG_STM32_HAVE_OTGFS is not set
# CONFIG_STM32_HAVE_FSMC is not set
CONFIG_STM32_HAVE_USART3=y
CONFIG_STM32_HAVE_UART4=y
CONFIG_STM32_HAVE_UART5=y
# CONFIG_STM32_HAVE_USART6 is not set
# CONFIG_STM32_HAVE_UART7 is not set
# CONFIG_STM32_HAVE_UART8 is not set
CONFIG_STM32_HAVE_TIM1=y
CONFIG_STM32_HAVE_TIM5=y
CONFIG_STM32_HAVE_TIM6=y
CONFIG_STM32_HAVE_TIM7=y
CONFIG_STM32_HAVE_TIM8=y
# CONFIG_STM32_HAVE_TIM9 is not set
# CONFIG_STM32_HAVE_TIM10 is not set
# CONFIG_STM32_HAVE_TIM11 is not set
# CONFIG_STM32_HAVE_TIM12 is not set
# CONFIG_STM32_HAVE_TIM13 is not set
# CONFIG_STM32_HAVE_TIM14 is not set
# CONFIG_STM32_HAVE_TIM15 is not set
# CONFIG_STM32_HAVE_TIM16 is not set
# CONFIG_STM32_HAVE_TIM17 is not set
CONFIG_STM32_HAVE_ADC2=y
CONFIG_STM32_HAVE_ADC3=y
# CONFIG_STM32_HAVE_ADC4 is not set
CONFIG_STM32_HAVE_CAN1=y
# CONFIG_STM32_HAVE_CAN2 is not set
# CONFIG_STM32_HAVE_RNG is not set
# CONFIG_STM32_HAVE_ETHMAC is not set
CONFIG_STM32_HAVE_SPI2=y
CONFIG_STM32_HAVE_SPI3=y
# CONFIG_STM32_HAVE_SPI4 is not set
# CONFIG_STM32_HAVE_SPI5 is not set
# CONFIG_STM32_HAVE_SPI6 is not set
# CONFIG_STM32_ADC1 is not set
# CONFIG_STM32_ADC2 is not set
# CONFIG_STM32_ADC3 is not set
# CONFIG_STM32_BKP is not set
# CONFIG_STM32_CAN1 is not set
# CONFIG_STM32_CRC is not set
CONFIG_STM32_DMA1=y
CONFIG_STM32_DMA2=y
# CONFIG_STM32_DAC1 is not set
# CONFIG_STM32_DAC2 is not set
# CONFIG_STM32_I2C1 is not set
# CONFIG_STM32_I2C2 is not set
CONFIG_STM32_PWR=y
# CONFIG_STM32_SDIO is not set
# CONFIG_STM32_SPI1 is not set
CONFIG_STM32_SPI2=y
# CONFIG_STM32_SPI3 is not set
# CONFIG_STM32_TIM1 is not set
# CONFIG_STM32_TIM2 is not set
CONFIG_STM32_TIM3=y
# CONFIG_STM32_TIM4 is not set
# CONFIG_STM32_TIM5 is not set
# CONFIG_STM32_TIM6 is not set
# CONFIG_STM32_TIM7 is not set
# CONFIG_STM32_TIM8 is not set
CONFIG_STM32_USART1=y
# CONFIG_STM32_USART2 is not set
# CONFIG_STM32_USART3 is not set
# CONFIG_STM32_UART4 is not set
# CONFIG_STM32_UART5 is not set
# CONFIG_STM32_USB is not set
# CONFIG_STM32_IWDG is not set
# CONFIG_STM32_WWDG is not set
CONFIG_STM32_SPI=y
# CONFIG_STM32_CAN is not set

#
# Alternate Pin Mapping
#
# CONFIG_STM32_CAN1_NO_REMAP is not set
CONFIG_STM32_CAN1_REMAP1=y
# CONFIG_STM32_CAN1_REMAP2 is not set
# CONFIG_STM32_TIM1_NO_REMAP is not set
# CONFIG_STM32_TIM1_FULL_REMAP is not set
CONFIG_STM32_TIM1_PARTIAL_REMAP=y
# CONFIG_STM32_TIM3_NO_REMAP is not set
# CONFIG_STM32_TIM3_FULL_REMAP is not set
CONFIG_STM32_TIM3_PARTIAL_REMAP=y
# CONFIG_STM32_USART1_REMAP is not set
# CONFIG_STM32_JTAG_DISABLE is not set
CONFIG_STM32_JTAG_FULL_ENABLE=y
# CONFIG_STM32_JTAG_NOJNTRST_ENABLE is not set
# CONFIG_STM32_JTAG_SW_ENABLE is not set
CONFIG_STM32_DISABLE_IDLE_SLEEP_DURING_DEBUG=y
CONFIG_STM32_FORCEPOWER=y
# CONFIG_ARCH_BOARD_STM32_CUSTOM_CLOCKCONFIG is not set
# CONFIG_STM32_DMACAPABLE is not set
# CONFIG_STM32_TIM1_PWM is not set
# CONFIG_STM32_TIM3_PWM is not set
CONFIG_STM32_USART=y

#
# U[S]ART Configuration
#
# CONFIG_USART1_RS485 is not set
CONFIG_USART1_RXDMA=y
CONFIG_SERIAL_DISABLE_REORDERING=y
# CONFIG_STM32_FLOWCONTROL_BROKEN is not set
# CONFIG_STM32_USART_SINGLEWIRE is not set

#
# SPI Configuration
#
# CONFIG_STM32_SPI_INTERRUPTS is not set
CONFIG_STM32_SPI_DMA=y
# CONFIG_STM32_HAVE_RTC_SUBSECONDS is not set

#
# USB FS Host Configuration
#

#
# USB HS Host Configuration
#

#
# USB Host Debug Configuration
#

#
# USB Device Configuration
#

#
# CAN driver configuration
#

#
# Architecture Options
#
# CONFIG_ARCH_NOINTC is not set
# CONFIG_ARCH_VECNOTIRQ is not set
CONFIG_ARCH_DMA=y
CONFIG_ARCH_HAVE_IRQPRIO=y
# CONFIG_ARCH_L2CACHE is not set
# CONFIG_ARCH_HAVE_COHERENT_DCACHE is not set
# CONFIG_ARCH_HAVE_ADDRENV is not set
# CONFIG_ARCH_NEED_ADDRENV_MAPPING is not set
CONFIG_ARCH_HAVE_VFORK=y
# CONFIG_ARCH_HAVE_MMU is not set
CONFIG_ARCH_HAVE_MPU=y
# CONFIG_ARCH_NAND_HWECC is not set
# CONFIG_ARCH_HAVE_EXTCLK is not set
# CONFIG_ARCH_USE_MPU is not set
CONFIG_ARCH_IRQPRIO=y
CONFIG_ARCH_STACKDUMP=y
# CONFIG_ENDIAN_BIG is not set
# CONFIG_ARCH_IDLE_CUSTOM is not set
# CONFIG_ARCH_HAVE_RAMFUNCS is not set
CONFIG_ARCH_HAVE_RAMVECTORS=y
# CONFIG_ARCH_RAMVECTORS is not set

#
# Board Settings
#
CONFIG_BOARD_LOOPSPERMSEC=5483
# CONFIG_ARCH_CALIBRATION is not set

#
# Interrupt options
#
CONFIG_ARCH_HAVE_INTERRUPTSTACK=y
CONFIG_ARCH_INTERRUPTSTACK=360
CONFIG_ARCH_HAVE_HIPRI_INTERRUPT=y
CONFIG_ARCH_HIPRI_INTERRUPT=y

#
# Boot options
#
# CONFIG_BOOT_RUNFROMEXTSRAM is not set
CONFIG_BOOT_RUNFROMFLASH=y
# CONFIG_BOOT_RUNFROMISRAM is not set
# CONFIG_BOOT_RUNFROMSDRAM is not set
# CONFIG_BOOT_COPYTORAM is not set

#
# Boot Memory Configuration
#
CONFIG_RAM_START=0x20000000
CONFIG_RAM_SIZE=20480
# CONFIG_ARCH_HAVE_SDRAM is not set

#
# Board Selection
#
# CONFIG_ARCH_BOARD_MAPLE is not set
CONFIG_ARCH_BOARD_PX4CANNODE_V1=y
# CONFIG_ARCH_BOARD_CUSTOM is not set
CONFIG_ARCH_BOARD="px4cannode-v1"

#
# Common Board Options
#
CONFIG_ARCH_HAVE_LEDS=y
CONFIG_ARCH_LEDS=y
CONFIG_ARCH_HAVE_BUTTONS=y
CONFIG_ARCH_BUTTONS=y
CONFIG_ARCH_HAVE_IRQBUTTONS=y
CONFIG_NSH_MMCSDMINOR=0

#
# Board-Specific Options
#

#
# RTOS Features
#
# CONFIG_DISABLE_OS_API is not set

#
# Clocks and Timers
#
CONFIG_USEC_PER_TICK=10000
# CONFIG_SYSTEM_TIME64 is not set
# CONFIG_CLOCK_MONOTONIC is not set
# CONFIG_JULIAN_TIME is not set
CONFIG_START_YEAR=2014
CONFIG_START_MONTH=11
CONFIG_START_DAY=20
CONFIG_MAX_WDOGPARMS=2
CONFIG_PREALLOC_WDOGS=4
CONFIG_WDOG_INTRESERVE=2
CONFIG_PREALLOC_TIMERS=2

#
# Tasks and Scheduling
#
# CONFIG_INIT_NONE is not set
CONFIG_INIT_ENTRYPOINT=y
# CONFIG_INIT_FILEPATH is not set
CONFIG_USER_ENTRYPOINT="nsh_main"
CONFIG_RR_INTERVAL=0
CONFIG_TASK_NAME_SIZE=12
CONFIG_MAX_TASKS=4
# CONFIG_SCHED_HAVE_PARENT is not set
CONFIG_SCHED_WAITPID=y

#
# Pthread Options
#
# CONFIG_MUTEX_TYPES is not set
CONFIG_NPTHREAD_KEYS=0

#
# Performance Monitoring
#
# CONFIG_SCHED_CPULOAD is not set
CONFIG_SCHED_INSTRUMENTATION=y

#
# Files and I/O
#
CONFIG_DEV_CONSOLE=y
# CONFIG_FDCLONE_DISABLE is not set
CONFIG_FDCLONE_STDIO=y
CONFIG_SDCLONE_DISABLE=y
CONFIG_NFILE_DESCRIPTORS=5
CONFIG_NFILE_STREAMS=1
CONFIG_NAME_MAX=8
CONFIG_PRIORITY_INHERITANCE=y
CONFIG_SEM_PREALLOCHOLDERS=4
CONFIG_SEM_NNESTPRIO=4

#
# RTOS hooks
#
CONFIG_BOARD_INITIALIZE=y
# CONFIG_BOARD_INITTHREAD is not set
# CONFIG_SCHED_STARTHOOK is not set
CONFIG_SCHED_ATEXIT=y
CONFIG_SCHED_ATEXIT_MAX=1

#
# Signal Numbers
#
CONFIG_SIG_SIGUSR1=1
CONFIG_SIG_SIGUSR2=2
CONFIG_SIG_SIGALARM=3
CONFIG_SIG_SIGCONDTIMEDOUT=16
CONFIG_SIG_SIGWORK=17

#
# POSIX Message Queue Options
#
CONFIG_PREALLOC_MQ_MSGS=4
CONFIG_MQ_MAXMSGSIZE=8

#
# Work Queue Support
#
# CONFIG_SCHED_WORKQUEUE is not set
# CONFIG_SCHED_HPWORK is not set
# CONFIG_SCHED_HPWORKPRIORITY is not set
# CONFIG_SCHED_HPWORKPERIOD is not set
# CONFIG_SCHED_HPWORKSTACKSIZE is not set
# CONFIG_SCHED_LPWORK is not set

#
# Stack and heap information
#
CONFIG_IDLETHREAD_STACKSIZE=272
CONFIG_USERMAIN_STACKSIZE=680
CONFIG_PTHREAD_STACK_MIN=516
CONFIG_PTHREAD_STACK_DEFAULT=516
# CONFIG_LIB_SYSCALL is not set

#
# Device Drivers
#
# CONFIG_DISABLE_POLL is not set
# CONFIG_DEV_NULL is not set
# CONFIG_DEV_ZERO is not set
# CONFIG_LOOP is not set

#
# Buffering
#
# CONFIG_DRVR_WRITEBUFFER is not set
# CONFIG_DRVR_READAHEAD is not set
# CONFIG_RAMDISK is not set
# CONFIG_CAN is not set
# CONFIG_ARCH_HAVE_PWM_PULSECOUNT is not set
# CONFIG_PWM is not set
CONFIG_ARCH_HAVE_I2CRESET=y
# CONFIG_I2C is not set
CONFIG_SPI=y
# CONFIG_SPI_OWNBUS is not set
CONFIG_SPI_EXCHANGE=y
# CONFIG_SPI_CMDDATA is not set
# CONFIG_SPI_CALLBACK is not set
# CONFIG_SPI_BITBANG is not set
# CONFIG_I2S is not set
# CONFIG_TIMER is not set
# CONFIG_RTC is not set
# CONFIG_WATCHDOG is not set
# CONFIG_ANALOG is not set
# CONFIG_AUDIO_DEVICES is not set
# CONFIG_VIDEO_DEVICES is not set
# CONFIG_BCH is not set
# CONFIG_INPUT is not set
# CONFIG_LCD is not set
# CONFIG_MMCSD is not set
# CONFIG_MTD is not set
# CONFIG_EEPROM is not set
# CONFIG_PIPES is not set
# CONFIG_PM is not set
# CONFIG_POWER is not set
# CONFIG_SENSORS is not set
# CONFIG_SERCOMM_CONSOLE is not set
CONFIG_SERIAL=y
# CONFIG_DEV_LOWCONSOLE is not set
# CONFIG_16550_UART is not set
# CONFIG_ARCH_HAVE_UART is not set
# CONFIG_ARCH_HAVE_UART0 is not set
# CONFIG_ARCH_HAVE_UART1 is not set
# CONFIG_ARCH_HAVE_UART2 is not set
# CONFIG_ARCH_HAVE_UART3 is not set
# CONFIG_ARCH_HAVE_UART4 is not set
# CONFIG_ARCH_HAVE_UART5 is not set
# CONFIG_ARCH_HAVE_UART6 is not set
# CONFIG_ARCH_HAVE_UART7 is not set
# CONFIG_ARCH_HAVE_UART8 is not set
# CONFIG_ARCH_HAVE_SCI0 is not set
# CONFIG_ARCH_HAVE_SCI1 is not set
# CONFIG_ARCH_HAVE_USART0 is not set
CONFIG_ARCH_HAVE_USART1=y
# CONFIG_ARCH_HAVE_USART2 is not set
# CONFIG_ARCH_HAVE_USART3 is not set
# CONFIG_ARCH_HAVE_USART4 is not set
# CONFIG_ARCH_HAVE_USART5 is not set
# CONFIG_ARCH_HAVE_USART6 is not set
# CONFIG_ARCH_HAVE_USART7 is not set
# CONFIG_ARCH_HAVE_USART8 is not set
# CONFIG_ARCH_HAVE_OTHER_UART is not set

#
# USART Configuration
#
CONFIG_USART1_ISUART=y
CONFIG_MCU_SERIAL=y
CONFIG_STANDARD_SERIAL=y
CONFIG_SERIAL_NPOLLWAITERS=2
# CONFIG_SERIAL_IFLOWCONTROL is not set
# CONFIG_SERIAL_OFLOWCONTROL is not set
# CONFIG_SERIAL_TIOCSERGSTRUCT is not set
CONFIG_ARCH_HAVE_SERIAL_TERMIOS=y
# CONFIG_SERIAL_TERMIOS is not set
CONFIG_USART1_SERIAL_CONSOLE=y
# CONFIG_OTHER_SERIAL_CONSOLE is not set
# CONFIG_NO_SERIAL_CONSOLE is not set

#
# USART1 Configuration
#
CONFIG_USART1_RXBUFSIZE=32
CONFIG_USART1_TXBUFSIZE=32
CONFIG_USART1_BAUD=115200
CONFIG_USART1_BITS=8
CONFIG_USART1_PARITY=0
CONFIG_USART1_2STOP=0
# CONFIG_USART1_IFLOWCONTROL is not set
# CONFIG_USART1_OFLOWCONTROL is not set
# CONFIG_USBDEV is not set
# CONFIG_USBHOST is not set
# CONFIG_WIRELESS is not set

#
# System Logging Device Options
#

#
# System Logging
#
# CONFIG_RAMLOG is not set
# CONFIG_SYSLOG_CONSOLE is not set

#
# Networking Support
#
# CONFIG_ARCH_HAVE_NET is not set
# CONFIG_ARCH_HAVE_PHY is not set
# CONFIG_NET is not set

#
# Crypto API
#
# CONFIG_CRYPTO is not set

#
# File Systems
#

#
# File system configuration
#
# CONFIG_DISABLE_MOUNTPOINT is not set
# CONFIG_FS_AUTOMOUNTER is not set
# CONFIG_DISABLE_PSEUDOFS_OPERATIONS is not set
# CONFIG_FS_READABLE is not set
# CONFIG_FS_WRITABLE is not set
# CONFIG_FS_NAMED_SEMAPHORES is not set
CONFIG_FS_MQUEUE_MPATH="/var/mqueue"
# CONFIG_FS_RAMMAP is not set
# CONFIG_FS_FAT is not set
# CONFIG_FS_NXFFS is not set
# CONFIG_FS_ROMFS is not set
# CONFIG_FS_SMARTFS is not set
# CONFIG_FS_BINFS is not set
# CONFIG_FS_PROCFS is not set

#
# System Logging
#
# CONFIG_SYSLOG is not set
# CONFIG_SYSLOG_TIMESTAMP is not set

#
# Graphics Support
#
# CONFIG_NX is not set

#
# Memory Management
#
CONFIG_MM_SMALL=y
CONFIG_MM_REGIONS=1
# CONFIG_ARCH_HAVE_HEAP2 is not set
# CONFIG_GRAN is not set

#
# Audio Support
#
# CONFIG_AUDIO is not set

#
# Binary Loader
#
# CONFIG_BINFMT_DISABLE is not set
# CONFIG_BINFMT_EXEPATH is not set
# CONFIG_NXFLAT is not set
# CONFIG_ELF is not set
CONFIG_BUILTIN=y
# CONFIG_PIC is not set
CONFIG_SYMTAB_ORDEREDBYNAME=y

#
# Library Routines
#

#
# Standard C Library Options
#
CONFIG_STDIO_BUFFER_SIZE=64
CONFIG_STDIO_LINEBUFFER=y
CONFIG_NUNGET_CHARS=2
CONFIG_LIB_HOMEDIR="/"
# CONFIG_NOPRINTF_FIELDWIDTH is not set
# CONFIG_LIBC_FLOATINGPOINT is not set
# CONFIG_LIBC_IOCTL_VARIADIC is not set
CONFIG_LIB_RAND_ORDER=1
# CONFIG_EOL_IS_CR is not set
# CONFIG_EOL_IS_LF is not set
# CONFIG_EOL_IS_BOTH_CRLF is not set
CONFIG_EOL_IS_EITHER_CRLF=y
# CONFIG_LIBC_EXECFUNCS is not set
CONFIG_POSIX_SPAWN_PROXY_STACKSIZE=394
CONFIG_TASK_SPAWN_DEFAULT_STACKSIZE=394
# CONFIG_LIBC_STRERROR is not set
# CONFIG_LIBC_PERROR_STDOUT is not set
CONFIG_ARCH_LOWPUTC=y
# CONFIG_LIBC_LOCALTIME is not set
CONFIG_LIB_SENDFILE_BUFSIZE=0
# CONFIG_ARCH_ROMGETC is not set
# CONFIG_ARCH_OPTIMIZED_FUNCTIONS is not set

#
# Non-standard Library Support
#
# CONFIG_LIB_KBDCODEC is not set
# CONFIG_LIB_SLCDCODEC is not set

#
# Basic CXX Support
#
CONFIG_C99_BOOL8=y
CONFIG_HAVE_CXX=y
CONFIG_HAVE_CXXINITIALIZE=y
# CONFIG_CXX_NEWLONG is not set

#
# uClibc++ Standard C++ Library
#
# CONFIG_UCLIBCXX is not set

#
# Application Configuration
#

#
# Built-In Applications
#
CONFIG_BUILTIN_PROXY_STACKSIZE=392

#
# Examples
#
# CONFIG_EXAMPLES_BUTTONS is not set
# CONFIG_EXAMPLES_CAN is not set
# CONFIG_EXAMPLES_CONFIGDATA is not set
# CONFIG_EXAMPLES_CPUHOG is not set
# CONFIG_EXAMPLES_CXXTEST is not set
# CONFIG_EXAMPLES_DHCPD is not set
# CONFIG_EXAMPLES_ELF is not set
# CONFIG_EXAMPLES_FTPC is not set
# CONFIG_EXAMPLES_FTPD is not set
# CONFIG_EXAMPLES_HELLO is not set
# CONFIG_EXAMPLES_HELLOXX is not set
# CONFIG_EXAMPLES_JSON is not set
# CONFIG_EXAMPLES_HIDKBD is not set
# CONFIG_EXAMPLES_KEYPADTEST is not set
# CONFIG_EXAMPLES_IGMP is not set
# CONFIG_EXAMPLES_MM is not set
# CONFIG_EXAMPLES_MODBUS is not set
# CONFIG_EXAMPLES_MOUNT is not set
# CONFIG_EXAMPLES_NRF24L01TERM is not set
CONFIG_EXAMPLES_NSH=y
# CONFIG_EXAMPLES_NSH_CXXINITIALIZE is not set
CONFIG_EXAMPLES_NULL=y
# CONFIG_EXAMPLES_NX is not set
# CONFIG_EXAMPLES_NXTERM is not set
# CONFIG_EXAMPLES_NXFFS is not set
# CONFIG_EXAMPLES_NXFLAT is not set
# CONFIG_EXAMPLES_NXHELLO is not set
# CONFIG_EXAMPLES_NXIMAGE is not set
# CONFIG_EXAMPLES_NXLINES is not set
# CONFIG_EXAMPLES_NXTEXT is not set
# CONFIG_EXAMPLES_OSTEST is not set
# CONFIG_EXAMPLES_PIPE is not set
# CONFIG_EXAMPLES_POSIXSPAWN is not set
# CONFIG_EXAMPLES_QENCODER is not set
# CONFIG_EXAMPLES_RGMP is not set
# CONFIG_EXAMPLES_ROMFS is not set
# CONFIG_EXAMPLES_SENDMAIL is not set
# CONFIG_EXAMPLES_SERIALBLASTER is not set
# CONFIG_EXAMPLES_SERIALRX is not set
# CONFIG_EXAMPLES_SERLOOP is not set
# CONFIG_EXAMPLES_SLCD is not set
# CONFIG_EXAMPLES_SMART_TEST is not set
# CONFIG_EXAMPLES_SMART is not set
# CONFIG_EXAMPLES_TCPECHO is not set
# CONFIG_EXAMPLES_TELNETD is not set
# CONFIG_EXAMPLES_THTTPD is not set
# CONFIG_EXAMPLES_TIFF is not set
# CONFIG_EXAMPLES_TOUCHSCREEN is not set
# CONFIG_EXAMPLES_WEBSERVER is not set
# CONFIG_EXAMPLES_USBSERIAL is not set
# CONFIG_EXAMPLES_USBTERM is not set
# CONFIG_EXAMPLES_WATCHDOG is not set

#
# Graphics Support
#
# CONFIG_TIFF is not set
# CONFIG_GRAPHICS_TRAVELER is not set

#
# Interpreters
#
# CONFIG_INTERPRETERS_FICL is not set
# CONFIG_INTERPRETERS_PCODE is not set
# CONFIG_INTERPRETERS_MICROPYTHON is not set

#
# Network Utilities
#

#
# Networking Utilities
#
# CONFIG_NETUTILS_CODECS is not set
# CONFIG_NETUTILS_FTPC is not set
# CONFIG_NETUTILS_JSON is not set
# CONFIG_NETUTILS_SMTP is not set
# CONFIG_NETUTILS_TFTPC is not set
# CONFIG_NETUTILS_THTTPD is not set
# CONFIG_NETUTILS_NETLIB is not set
# CONFIG_NETUTILS_WEBCLIENT is not set
# CONFIG_NETUTILS_PPPD is not set

#
# FreeModBus
#
# CONFIG_MODBUS is not set

#
# NSH Library
#
CONFIG_NSH_LIBRARY=y

#
# Command Line Configuration
#
CONFIG_NSH_READLINE=y
# CONFIG_NSH_CLE is not set
CONFIG_NSH_LINELEN=25
CONFIG_NSH_DISABLE_SEMICOLON=y
# CONFIG_NSH_CMDPARMS is not set
CONFIG_NSH_MAXARGUMENTS=4
# CONFIG_NSH_ARGCAT is not set
CONFIG_NSH_NESTDEPTH=0
CONFIG_NSH_DISABLEBG=y
CONFIG_NSH_BUILTIN_APPS=y

#
# Disable Individual commands
#
CONFIG_NSH_DISABLE_ADDROUTE=y
# CONFIG_NSH_DISABLE_CAT is not set
CONFIG_NSH_DISABLE_CD=y
CONFIG_NSH_DISABLE_CP=y
CONFIG_NSH_DISABLE_CMP=y
CONFIG_NSH_DISABLE_DD=y
CONFIG_NSH_DISABLE_DF=y
CONFIG_NSH_DISABLE_DELROUTE=y
# CONFIG_NSH_DISABLE_ECHO is not set
CONFIG_NSH_DISABLE_EXEC=y
CONFIG_NSH_DISABLE_EXIT=y
# CONFIG_NSH_DISABLE_FREE is not set
CONFIG_NSH_DISABLE_GET=y
# CONFIG_NSH_DISABLE_HELP is not set
CONFIG_NSH_DISABLE_HEXDUMP=y
CONFIG_NSH_DISABLE_IFCONFIG=y
CONFIG_NSH_DISABLE_KILL=y
CONFIG_NSH_DISABLE_LOSETUP=y
# CONFIG_NSH_DISABLE_LS is not set
CONFIG_NSH_DISABLE_MB=y
CONFIG_NSH_DISABLE_MKDIR=y
CONFIG_NSH_DISABLE_MKFIFO=y
CONFIG_NSH_DISABLE_MKRD=y
CONFIG_NSH_DISABLE_MH=y
CONFIG_NSH_DISABLE_MOUNT=y
CONFIG_NSH_DISABLE_MV=y
CONFIG_NSH_DISABLE_MW=y
CONFIG_NSH_DISABLE_PS=y
CONFIG_NSH_DISABLE_PUT=y
CONFIG_NSH_DISABLE_PWD=y
CONFIG_NSH_DISABLE_RM=y
CONFIG_NSH_DISABLE_RMDIR=y
CONFIG_NSH_DISABLE_SET=y
CONFIG_NSH_DISABLE_SH=y
CONFIG_NSH_DISABLE_SLEEP=y
CONFIG_NSH_DISABLE_TEST=y
CONFIG_NSH_DISABLE_UMOUNT=y
CONFIG_NSH_DISABLE_UNSET=y
CONFIG_NSH_DISABLE_USLEEP=y
CONFIG_NSH_DISABLE_WGET=y
CONFIG_NSH_DISABLE_XD=y

#
# Configure Command Options
#
CONFIG_NSH_CODECS_BUFSIZE=0
# CONFIG_NSH_CMDOPT_HEXDUMP is not set
CONFIG_NSH_FILEIOSIZE=64

#
# Scripting Support
#
CONFIG_NSH_DISABLESCRIPT=y

#
# Console Configuration
#
CONFIG_NSH_CONSOLE=y
# CONFIG_NSH_ALTCONDEV is not set
CONFIG_NSH_ARCHINIT=y

#
# NxWidgets/NxWM
#

#
# Platform-specific Support
#
# CONFIG_PLATFORM_CONFIGDATA is not set

#
# System Libraries and NSH Add-Ons
#

#
# Custom Free Memory Command
#
# CONFIG_SYSTEM_FREE is not set

#
# EMACS-like Command Line Editor
#
# CONFIG_SYSTEM_CLE is not set

#
# CU Minimal Terminal
#
# CONFIG_SYSTEM_CUTERM is not set

#
# FLASH Program Installation
#
# CONFIG_SYSTEM_INSTALL is not set

#
# FLASH Erase-all Command
#

#
# Intel HEX to binary conversion
#
# CONFIG_SYSTEM_HEX2BIN is not set

#
# I2C tool
#

#
# INI File Parser
#
# CONFIG_SYSTEM_INIFILE is not set

#
# NxPlayer media player library / command Line
#

#
# RAM test
#
# CONFIG_SYSTEM_RAMTEST is not set

#
# readline()
#
CONFIG_SYSTEM_READLINE=y
CONFIG_READLINE_ECHO=y

#
# P-Code Support
#

#
# PHY Tool
#

#
# Power Off
#
# CONFIG_SYSTEM_POWEROFF is not set

#
# RAMTRON
#
# CONFIG_SYSTEM_RAMTRON is not set

#
# SD Card
#
# CONFIG_SYSTEM_SDCARD is not set

#
# Sudoku
#
# CONFIG_SYSTEM_SUDOKU is not set

#
# Sysinfo
#
# CONFIG_SYSTEM_SYSINFO is not set

#
# Temperature
#

#
# VI Work-Alike Editor
#
# CONFIG_SYSTEM_VI is not set

#
# Stack Monitor
#
# CONFIG_SYSTEM_STACKMONITOR is not set

#
# USB CDC/ACM Device Commands
#

#
# USB Composite Device Commands
#

#
# USB Mass Storage Device Commands
#

#
# USB Monitor
#

#
# Zmodem Commands
#
# CONFIG_SYSTEM_ZMODEM is not set
