#ifndef THERMO_H
#define THERMO_H


#include "i2c.h"
#include "stdint.h"


uint8_t DevAddress = 0x48 << 1;
uint8_t rBuf[2];


float calcTemp(uint8_t buf[]) {

	return 0.0f;
}

float readTemp()
{
	if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, rBuf, 2, HAL_MAX_DELAY))
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
