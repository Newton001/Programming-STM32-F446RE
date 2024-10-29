/*
 * thermo.c
 *
 *  Created on: Nov 1, 2021
 *      Author: Patrik Arnold
 */

#ifndef THERMO_C_
#define THERMO_C_

#include "thermo.h"
#include "i2c.h"
#include "stm32f4xx_hal.h"

#define THERMO_ADDRESS 0x90
#define TEST_MODE 0 // Macro for enable testing without sensor / Freezer

float calcTemp(uint16_t data)
{
	data = (data>>5);

	if(data & 0x400) 							// Check sign bit (bit 11)
		return -0.125f*((~data + 1) & 0x3FF); // Two's complement, convert to 10 bit value, scale to °C

	return 0.125f * data;						// scale to °C
}

#if TEST_MODE

float readTemp()
{
//uint8_t i2cBuf[2] = {0x19,0x00}; // 0001 1001 0000 0000 is  25°C
	uint8_t i2cBuf[2] = {0xE7,0x00}; // 1110 0111 0000 0000 is -25°C

	uint16_t tmp = (i2cBuf[0]<<8) + i2cBuf[1]; // assemble 16-bit temp value
	return calcTemp(tmp);
}
#else
float readTemp()
{
	uint8_t i2cBuf[2];
	if(HAL_I2C_Master_Receive(&hi2c1, THERMO_ADDRESS, (uint8_t*)i2cBuf, 2, HAL_MAX_DELAY)==HAL_OK)
	{
		uint16_t tmp = (i2cBuf[0]<<8) + i2cBuf[1];   // assemble 16-bit temp value
		return calcTemp(tmp);
	}
	return 1500.f; // return something out of range

}
#endif // TESTING


#endif /* THERMO_C_ */
