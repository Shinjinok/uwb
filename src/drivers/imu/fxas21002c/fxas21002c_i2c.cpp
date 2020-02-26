/****************************************************************************
 *
 *   Copyright (c) 2016-2020 PX4 Development Team. All rights reserved.
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
 * @file fxas21002c_i2c.cpp
 *
 * Driver for the NXP FXAS21002C connected via I2C.
 *
 * @author Robert Fu
 */
#include <drivers/device/i2c.h>
#include "FXAS21002C.hpp"

#define FXAS21002C_I2C_BUS_SPEED    400 * 1000

device::Device *FXAS21002C_I2C_interface(int bus, uint32_t slave_address);

class FXAS21002C_I2C : public device::I2C
{
public:
	FXAS21002C_I2C(int bus, uint32_t slave_address);
	~FXAS21002C_I2C() override = default;

	/**
	 * Read directly from the device.
	 *
	 * The actual size of each unit quantity is device-specific.
	 *
	 * @param reg	The register address at which to start reading
	 * @param data	The buffer into which the read values should be placed.
	 * @param count	The number of items to read.
	 * @return		The number of items read on success, negative errno otherwise.
	 */
	int	read(unsigned reg, void *data, unsigned count) override;

	/**
	 * Write directly to the device.
	 *
	 * The actual size of each unit quantity is device-specific.
	 *
	 * @param reg	The register address at which to start writing.
	 * @param data	The buffer from which values should be read.
	 * @param count	The number of items to write.
	 * @return		The number of items written on success, negative errno otherwise.
	 */
	int	write(unsigned reg, void *data, unsigned count) override;

protected:
	int probe() override;
};

device::Device *
FXAS21002C_I2C_interface(int bus, uint32_t slave_address)
{
	return new FXAS21002C_I2C(bus, slave_address);
}

FXAS21002C_I2C::FXAS21002C_I2C(int bus, uint32_t slave_address) :
	I2C("FXAS21002C", nullptr, bus, slave_address, FXAS21002C_I2C_BUS_SPEED)
{
	set_device_type(DRV_GYR_DEVTYPE_FXAS2100C);
}

int
FXAS21002C_I2C::probe()
{
	uint8_t whoami = read_reg(FXAS21002C_WHO_AM_I);
	bool success = (whoami == WHO_AM_I);

	PX4_INFO("FXAS21002C_I2C::probe: %s, whoami: 0x%02x", (success ? "Succeeded" : "failed"), whoami);
	return success ? OK : -EIO;
}

/**
 * Read directly from the device.
 *
 * The actual size of each unit quantity is device-specific.
 *
 * @param reg	The register address at which to start reading
 * @param data	The buffer into which the read values should be placed.
 * @param count	The number of items to read.
 * @return		The number of items read on success, negative errno otherwise.
 */
int FXAS21002C_I2C::read(unsigned reg, void *data, unsigned count)
{
	/* Same as in mpu9250_i2c.cpp:
	 * We want to avoid copying the data of RawGyroReport: So if the caller
	 * supplies a buffer not RawGyroReport in size, it is assume to be a reg or
	 * reg 16 read
	 * Since RawGyroReport has a cmd at front, we must return the data
	 * after that. Foe anthing else we must return it
	 */
	uint32_t offset = count < sizeof(RawGyroReport) ? 0 : offsetof(RawGyroReport, status);
	uint8_t cmd = FXAS21002C_REG(reg);

	return transfer(&cmd, 1, &((uint8_t *)data)[offset], count - offset);
}

/**
 * Write directly to the device.
 *
 * The actual size of each unit quantity is device-specific.
 *
 * @param reg	The register address at which to start writing.
 * @param data	The buffer from which values should be read.
 * @param count	The number of items to write.
 * @return		The number of items written on success, negative errno otherwise.
 */
int FXAS21002C_I2C::write(unsigned reg, void *data, unsigned count)
{
	uint8_t cmd[2];

	if (sizeof(cmd) < (count + 1)) {
		// same as in mpu9250_i2c.cpp
		// This condition means only supportting the case of count == 1
		// so this API is the same as write_reg
		return -EIO;
	}

	cmd[0] = FXAS21002C_REG(reg);
	cmd[1] = *(uint8_t *)data;

	return transfer(cmd, sizeof(cmd), nullptr, 0);
}

