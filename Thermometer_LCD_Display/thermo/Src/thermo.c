#ifndef THERMO_H
#define THERMO_H


#include "i2c.h"
#include "stdint.h"


uint8_t DevAddress = 0x48 << 1;
uint8_t rBuf[2];

float calcTemp(uint16_t data)
{
	data = (data>>5);

	if(data & 0x400) 							// Check sign bit (bit 11)
		return -0.125f*((~data + 1) & 0x3FF); // Two's complement, convert to 10 bit value, scale to °C

	return 0.125f * data;						// scale to °C
}

float readTemp()
{
	uint8_t i2cBuf[2];
	if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, (uint8_t*)i2cBuf, 2, HAL_MAX_DELAY)==HAL_OK)
	{
		uint16_t tmp = (i2cBuf[0]<<8) + i2cBuf[1];   // assemble 16-bit temp value
		return calcTemp(tmp);
	}
	return 1500.f; // return something out of range

}

float readTemp2()
{
	if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, rBuf, 2, HAL_MAX_DELAY)==HAL_OK)
	{
		int16_t raw_temp = (rBuf[0] << 8) | rBuf[1]; // Combine Bytes
		raw_temp >>= 5;  // Right shift to align the 11-bit two's complement value

		// Check if the temperature is negative (two's complement)
		if (raw_temp & 0x0400) // Check if bit 10 is set for negative temperature
		{
			raw_temp |= 0xF800; // Sign extend to 16 bits
		}

		// Convert to Celsius. Each bit represents 0.125°C
		float temp = raw_temp * 0.125f;
		return temp;
		//return 23.5f;
	}
	else
	{
		return 1000.0f;
	}
}

#endif
