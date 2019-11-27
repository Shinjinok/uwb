/**
 * @file ads1115.cpp
 *
 * Driver for the ADS1115-based power monitor board for Leap Aeronautics.
 *
 * TO-DO-
 *
 * 1) Complete Constructor, Destructor
 * 4) Add computation functions: Reverse 2's compliment, Averaging, Steinhart
 * 5) Sort out Read/Write processes, move on from confirming init() and probe() work.
 */

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/time.h>

#include <drivers/device/i2c.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include <nuttx/arch.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <nuttx/clock.h>

#include <board_config.h>

#include <perf/perf_counter.h>
#include <systemlib/err.h>

#include <drivers/drv_mag.h>
#include <drivers/drv_hrt.h>
#include <drivers/device/ringbuffer.h>
#include <drivers/drv_device.h>

#include <uORB/uORB.h>
#include <uORB/topics/power_monitor.h> // power_monitor struct msg
#include <uORB/topics/battery_status.h>

#include <float.h>
#include <px4_platform_common/getopt.h> // Reading inputs from NuttX shell
#include <lib/conversion/rotation.h>

// #include "ads1115.h"

#define ADS1115_BUS_DEFAULT     PX4_I2C_BUS_EXPANSION
#define ADS1115_BASEADDR        (0x48)

#define ADS1115_CONVERSION_INTERVAL     (8000) // 1/Data Rate in microseconds

#define ADS1115_WRITE_ADDR      (0x90) // 1001 000(0)
#define ADS1115_READ_ADDR       (0x91) // 1001 000(1)

#define ADS1115_CONFIG_REG      (0x01)
#define ADS1115_CONVERSION_REG  (0x00)

#define ADS1115_CHANNEL_ONE     (0xC383) // 1 100 001 1 100 0 0 0 11
#define ADS1115_CHANNEL_TWO     (0xD383) // 1 101 001 1 100 0 0 0 11
#define ADS1115_CHANNEL_THREE 	(0xE383) // 1 110 001 1 100 0 0 0 11
#define ADS1115_CHANNEL_FOUR	(0xF383) // 1 111 001 1 100 0 0 0 11

#define swap16(w)       __builtin_bswap16((w))

#define v_offset                (0.364) // Value measured without source

class ADS1115 : public device::I2C, px4::ScheduledWorkItem
{
public:
	ADS1115(int bus = ADS1115_BUS_DEFAULT, int address = ADS1115_BASEADDR);
	virtual ~ADS1115();

	virtual int init();

	void print_info();

    void start();

    void stop();

protected:
	virtual int probe();

private:
	orb_advert_t _power_monitor_topic{nullptr};
    orb_advert_t _battery_status_topic{nullptr};

	float _measure_interval{ADS1115_CONVERSION_INTERVAL};

	int writeConfigReg(uint16_t value);
	int readConfigReg(uint16_t *configRegValue);
	int readConvReg(uint16_t *convRegValue);

	double getVoltage();
	double getCurrent();
	double getWireTemp();
	double getBattTemp();

    double v_avging();
    double i_avging();

    void Run() override;
};

extern "C" __EXPORT int ads1115_main(int argc, char *argv[]);

ADS1115::ADS1115(int bus, int address) : // FILL CONSTRUCTOR
	I2C("ADS1115", nullptr, bus, address, 100000),
	ScheduledWorkItem(MODULE_NAME, px4::device_bus_to_wq(get_device_id()))
{
	PX4_DEBUG("constructor\n");
}

ADS1115::~ADS1115() //FILL DESTRUCTOR
{
    stop();
}

int ADS1115::readConvReg(uint16_t *convRegVal)
{
    uint8_t adrRegPtr = ADS1115_CONVERSION_REG;

	transfer(&adrRegPtr, sizeof(adrRegPtr), nullptr, 0);

    transfer(nullptr, 0, (uint8_t *)convRegVal, sizeof(convRegVal));

    // PX4_INFO("%u", swap16(&convRegVal));

    return PX4_OK;
}

int ADS1115::readConfigReg(uint16_t *configRegVal)
{
    uint8_t adrRegPtr = ADS1115_CONFIG_REG;

	transfer(&adrRegPtr, sizeof(adrRegPtr), nullptr, 0);

    transfer(nullptr, 0, (uint8_t *)configRegVal, sizeof(configRegVal));

    return PX4_OK;
}

int ADS1115::writeConfigReg(uint16_t value)
{
    uint8_t data[3] = {ADS1115_CONFIG_REG,
		((uint8_t)((value & 0xff00) >> 8)), (uint8_t)(value & 0xff)};

	transfer(data, sizeof(data), nullptr, 0);

    return PX4_OK;
}

int ADS1115::probe()
{
    uint16_t configRegVal = 0;
    uint16_t convRegVal = 0;

    writeConfigReg(ADS1115_CHANNEL_ONE);

    if (!readConfigReg(&configRegVal))
    {
        if (0x8343 != configRegVal)
        {
            PX4_WARN("ADS1115::readConfigReg() failed");
            return PX4_ERROR;
        } else {
            return PX4_OK;
        }
    }

    if (!readConvReg(&convRegVal))
    {
        PX4_INFO("%u", convRegVal);
    }

    PX4_INFO("ADS1115::probe() success");
    return PX4_OK;
}

int ADS1115::init()
{
    if (I2C::init() != PX4_OK)
    {
        PX4_WARN("I2C::init() failed, in ADS1115::init() if.1");
        return PX4_ERROR;
    }
    PX4_INFO("ADS1115::init() success");

    start();

    return PX4_OK;
}

void ADS1115::print_info()
{
    PX4_INFO("Driver information: ADS1115 Power Monitor Breakout Board");
}

// double ADS1115::getCurrent()
// {
//     writeConfigReg(ADS1115_CHANNEL_ONE);

//     uint16_t convRegVal = 0;
//     uint16_t confRegVal = 0;
//     double current = 0;

//     while (!readConfigReg(&confRegVal) | (0xC383 == confRegVal))
//     {
//         readConvReg(&convRegVal);
//         current = (swap16(convRegVal))*0.025;
//         break;
//     }
//     return current;
// }

// double ADS1115::v_avging()
// {
//     int n =5;
//     uint32_t sum = 0;
//     uint32_t average = 0;
//     uint16_t convRegVal = 0;

//     writeConfigReg(ADS1115_CHANNEL_TWO);

//     for (int i = 0; i < n; i++)
//     {

//         ScheduleDelayed(ADS1115_CONVERSION_INTERVAL);
//         if (!readConvReg(&convRegVal))
//         {
//             sum = sum + swap16(convRegVal);
//         } else
//         {
//             PX4_ERR("Voltage Averaging failed");
//         }

//     }
//     average = sum/n;
//     return average;
// }

// double ADS1115::i_avging()
// {
//     int n =5;
//     uint32_t sum = 0;
//     uint32_t average = 0;
//     uint16_t convRegVal = 0;

//     writeConfigReg(ADS1115_CHANNEL_ONE);

//     for (int i = 0; i < n; i++)
//     {
//         if (!readConvReg(&convRegVal))
//         {
//             sum = sum + swap16(convRegVal);
//             ScheduleDelayed(ADS1115_CONVERSION_INTERVAL);
//         } else
//         {
//             PX4_ERR("Current averaging failed");
//         }
//     }
//     average = sum/n;
//     return average;
// }

// double ADS1115::v_avging()
// {
//     int n = 5;
//     uint32_t sum = 0;
//     uint32_t average = 0;
//     uint16_t convRegVal;

//     for (int i = 0; i < n; i++)
//     {
//         readConvReg(&convRegVal);
//         sum = sum + swap16(convRegVal);
//         ScheduleDelayed(ADS1115_CONVERSION_INTERVAL);
//     }
//     average = sum/n;
//     return average;
// }

// double ADS1115::i_avging()
// {
//     int n = 5;
//     uint32_t sum = 0;
//     uint32_t average = 0;
//     uint16_t convRegVal;

//     for (int i = 0; i < n; i++)
//     {
//         readConvReg(&convRegVal);
//         sum = sum + swap16(convRegVal);
//         ScheduleDelayed(ADS1115_CONVERSION_INTERVAL);
//     }
//     average = sum/n;
//     return average;
// }

double ADS1115::getCurrent()
{
    double current = 0;
    uint16_t convRegVal = 0;

    writeConfigReg(ADS1115_CHANNEL_ONE);

    if (!readConvReg(&convRegVal))
    {
        current = (swap16(convRegVal))*(0.025);
        // current = (i_avging())*0.025; // ((avging())*(0.125/1000)/50)/0.1;
    } else
    {
        PX4_ERR("Current Reading Failed");
    }
    return current;
}

double ADS1115::getVoltage()
{
    double voltage = 0;
    uint16_t convRegVal = 0;

    writeConfigReg(ADS1115_CHANNEL_TWO);

    if (!readConvReg(&convRegVal))
    {
        voltage = ((swap16(convRegVal))*(0.004))-v_offset;
        // voltage = ((v_avging())*0.004); // (((avging())*0.125)/1000)/0.0323;
    } else
    {
        PX4_ERR("Voltage Reading Failed");
    }
    return voltage;
}

// double ADS1115::getWireTemp()
// {
//     double wiretemp = 0;
//     uint16_t convRegVal = 0;

//     writeConfigReg(ADS1115_CHANNEL_THREE);

//     if (!readConvReg(&convRegVal))
//     {
//         wiretemp = swap16(convRegVal);
//     } else
//     {
//         PX4_ERR("Wire Temperature Reading Failed");
//     }
//     return wiretemp;
// }

// double ADS1115::getBattTemp()
// {
//     double battTemp = 0;
//     uint16_t convRegVal = 0;

//     writeConfigReg(ADS1115_CHANNEL_FOUR);

//     if (!readConvReg(&convRegVal))
//     {
//         battTemp = swap16(convRegVal);
//     } else
//     {
//         PX4_ERR("Battery Temperature Reading Failed");
//     }
//     return battTemp;
// }

void ADS1115::start()
{
    ScheduleClear();
    Run();
    ScheduleDelayed(5);
}

void ADS1115::stop()
{
    ScheduleClear();
}

void ADS1115::Run()
{
    PX4_INFO("Run() in progress");
	struct power_monitor_s report;
	report.timestamp = hrt_absolute_time();
	report.voltage_v = getVoltage();
	//report.current_a = getCurrent();
	report.wiretemp_c = 0;
	report.batttemp_c = 0;

    struct battery_status_s battreport;
    battreport.timestamp = hrt_absolute_time();
    battreport.voltage_v = getVoltage();
    battreport.voltage_filtered_v = getVoltage();
    //battreport.current_a = getCurrent();

	int instance;
	orb_publish_auto(ORB_ID(power_monitor), &_power_monitor_topic,
        &report, &instance, ORB_PRIO_DEFAULT);

    int instance1;
    orb_publish_auto(ORB_ID(battery_status), &_battery_status_topic,
        &battreport, &instance1, ORB_PRIO_DEFAULT);

    ScheduleDelayed(100);
}
/* ------------------------------------------------------------------------- */

namespace ads1115 // COMPLETE SH_MEASURE()
{
    ADS1115 *g_dev;

    int sh_init(int i2c_bus);
    int sh_stop();
    int sh_info();

    int sh_init(int i2c_bus)
    {
        if (g_dev != nullptr)
        {
            PX4_ERR("ADS1115 Driver already started, g_dev != nullptr in sh_init if.1");
            return PX4_ERROR;
        }

        g_dev = new ADS1115(i2c_bus);

        if (g_dev == nullptr)
        {
            PX4_ERR("ADS1115 Driver not started, g_dev = nullptr in sh_init if.2");
            return PX4_ERROR;
        }

        if (OK != g_dev->init())
        {
            PX4_ERR("ADS1115::init() failed in sh_init if.3");
            goto fail;
        }

        // PX4_INFO("ADS1115 Driver sh_init() success");
        return PX4_OK;

        fail:
            if (g_dev != nullptr)
            {
                delete g_dev;
                g_dev = nullptr;
                return PX4_ERROR;
            }
        return PX4_ERROR;
    }

    int sh_stop()
	{
		if (g_dev != nullptr)
		{
			g_dev->stop();
            PX4_INFO("ADS1115 Driver sh_stop() success");
			// delete g_dev;
			g_dev = nullptr;
			return PX4_OK;
		} else
		{
			PX4_ERR("ADS1115 Driver not running yet");
			return PX4_ERROR;
		}
	}

	int sh_info()
	{
		if (g_dev == nullptr)
		{
			PX4_ERR("ADS1115 Driver not started, g_dev == nullptr in sh_info");
			return PX4_ERROR;
		}

		PX4_INFO("state @ %p\n", g_dev);
		g_dev->print_info();
		return PX4_OK;
	}
}

static void ads1115_usage()
{
    PX4_INFO("ads1115 [command]");
    PX4_INFO("commands:");
	PX4_INFO("\tinit|stop|info");
}

int ads1115_main(int argc, char *argv[])
{
    int ch;
	int myoptind = 1;
	const char *myoptarg = nullptr;
    int i2c_bus = ADS1115_BUS_DEFAULT;

	while ((ch = px4_getopt(argc, argv, "s:R:", &myoptind, &myoptarg)) != EOF)
    {
        switch(ch)
        {
            case 's':
                i2c_bus = atoi(myoptarg);
                break;
            default:
                PX4_WARN("unknown option!");
                goto fail;
        }

        if (!strcmp(argv[myoptind], "init"))
        {
            return ads1115::sh_init(i2c_bus);
        }

        if (!strcmp(argv[myoptind], "stop"))
        {
            return ads1115::sh_stop();
        }

	    if (!strcmp(argv[myoptind], "info"))
        {
            ads1115_usage();
	        return ads1115::sh_info();
        }

        fail:
            ads1115_usage();
            return PX4_ERROR;
    }
    return PX4_OK;
}
