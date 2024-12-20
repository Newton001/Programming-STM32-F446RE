/*
 * mma7660fc.c
 *
 *  Created on: Oct 29, 2024
 *      Author: newton
 */
#include "mma7660fc.h"
#include "stdio.h"

static float mma7660_scaling_factor = 1.5f / 32.0f; // Default to 1.5g range

// Simple function to detect device on the I2C bus
void sensorDetect(void) {
	HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(&hi2c1, DEVICE_ADDRESS, 1, 100);
	if (ret == HAL_OK) {
		printf("The Sensor is connected\n");
	} else {
		printf("The Device cannot be detected\n");
	}
}


// Initialize the MMA7660FC device
void mma7660_init(I2C_HandleTypeDef *hi2c) {
	mma7660_WriteRegister(hi2c, MODE_REG,0x00);
	mma7660_WriteRegister(hi2c, SR, 0x03);
	mma7660_WriteRegister(hi2c, MODE_REG,0x01);
}


// Read data from a single byte register
uint8_t mma7660_ReadRegister(I2C_HandleTypeDef *hi2c, uint8_t reg) {
	uint8_t data;
	if (HAL_I2C_Mem_Read(hi2c, DEVICE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY) == HAL_OK) {
		return data;
	}
	return 0xFF;
}


// Read acceleration data from X, Y, Z axes
//void mma7660_ReadAcceleration(I2C_HandleTypeDef *hi2c, uint8_t *x, uint8_t *y, uint8_t *z) {
//	*x = (uint8_t)mma7660_ReadRegister(hi2c, XOUT_REG);
//	*y = (uint8_t)mma7660_ReadRegister(hi2c, YOUT_REG);
//	*z = (uint8_t)mma7660_ReadRegister(hi2c, ZOUT_REG);
//}



HAL_StatusTypeDef mma7660_WriteRegister(I2C_HandleTypeDef *hi2c,uint8_t reg, uint8_t data) {
	return HAL_I2C_Mem_Write(&hi2c1, DEVICE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}


void mma7660_SetResolution(float range_g) {
    if (range_g == MMA7660_RANGE_1_5G) {
        mma7660_scaling_factor = 1.5f / 32.0f;
    } else if (range_g == MMA7660_RANGE_6G) {
        mma7660_scaling_factor = 6.0f / 32.0f;
    } else {
        // Handle invalid range
        printf("Invalid range selected.\n");
    }
}

void mma7660_ReadAcceleration(I2C_HandleTypeDef *hi2c, float *x_g, float *y_g, float *z_g) {
    // Read raw 6-bit data
    int8_t x_raw = mma7660_ReadRegister(hi2c, XOUT_REG);
    int8_t y_raw = mma7660_ReadRegister(hi2c, YOUT_REG);
    int8_t z_raw = mma7660_ReadRegister(hi2c, ZOUT_REG);

    // Mask and convert to two's complement
    x_raw = (x_raw & 0x3F);
    if (x_raw & 0x20) {
        x_raw = x_raw - 64;
    }
    y_raw = (y_raw & 0x3F);
    if (y_raw & 0x20) {
        y_raw = y_raw - 64;
    }
    z_raw = (z_raw & 0x3F);
    if (z_raw & 0x20) {
        z_raw = z_raw - 64;
    }

    // Convert raw values to g using the current scaling factor
    *x_g = x_raw * mma7660_scaling_factor;
    *y_g = y_raw * mma7660_scaling_factor;
    *z_g = z_raw * mma7660_scaling_factor;
}
