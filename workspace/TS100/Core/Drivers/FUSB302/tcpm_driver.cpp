/*
 * tcpm_driver.c
 *
 * Created: 11/11/2017 18:42:26
 *  Author: jason
 */

#include "tcpm_driver.h"
#include "I2C_Wrapper.hpp"
#include "I2CBB.hpp"
extern const struct tcpc_config_t tcpc_config;
#define STATUS_OK 0
/* I2C wrapper functions - get I2C port / slave addr from config struct. */
int tcpc_write(int reg, int val) {

	I2CBB::Mem_Write(tcpc_config.i2c_slave_addr, reg, (uint8_t*) &val, 1);
	return STATUS_OK;
}

int tcpc_write16(int reg, int val) {
	uint8_t data[2];
	data[0] = (0xFF) & val;
	data[1] = (0xFF) & (val >> 8);
	I2CBB::Mem_Write(tcpc_config.i2c_slave_addr, reg, (uint8_t*) data, 2);
	return STATUS_OK;
}

int tcpc_read(int reg, int *val) {
	uint8_t data[1];

	I2CBB::Mem_Read(tcpc_config.i2c_slave_addr, reg, (uint8_t*) data, 1);

	*val = data[0];

	return STATUS_OK;
}

int tcpc_read16(int reg, int *val) {
	uint8_t data[2];
	I2CBB::Mem_Write(tcpc_config.i2c_slave_addr, reg, (uint8_t*) data, 2);

	*val = data[0];
	*val |= (data[1] << 8);

	return STATUS_OK;
}

int tcpc_xfer(const uint8_t *out, int out_size, uint8_t *in, int in_size,
		int flags) {
	// Write out the I2C port to the given slave address
	// Write out the out byte array to the device (sending a stop if the flag is set)
	// Then issue a read from the device

	if (flags & I2C_XFER_STOP) {
		//Issuing a stop between the requests
		//Send as a Tx followed by a Rx
		I2CBB::Transmit(tcpc_config.i2c_slave_addr, (uint8_t*) out,
				out_size);
		I2CBB::Receive(tcpc_config.i2c_slave_addr, in, in_size);
	} else {
		//issue as a continious transmit & recieve
		I2CBB::TransmitReceive(tcpc_config.i2c_slave_addr, (uint8_t*) out,
				out_size, in, in_size);
	}

	return STATUS_OK;
}

uint8_t fusb302_detect() {
	//Probe the I2C bus for its address
	return I2CBB::probe(fusb302_I2C_SLAVE_ADDR);
}
