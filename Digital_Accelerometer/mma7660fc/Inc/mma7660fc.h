/*
 * mma7660fc.h
 *
 *  Created on: Oct 29, 2024
 *      Author: newton
 */

#ifndef MMA7660FC_H_
#define MMA7660FC_H_
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "i2c.h"

// Define the addresses to be used from the MMA7660FC
#define DEVICE_ADDRESS (0x4C << 1)
#define XOUT_REG 0x00
#define YOUT_REG 0x01
#define ZOUT_REG 0x02
#define TILT_REG 0x03 // Tilt Status
#define SRST_REG 0x04 // Sampling Rate Status
#define SPCNT    0x05 // Sleep Count
#define INTSU    0x06 // Interrupt Set Up
#define MODE_REG 0x07
#define SR       0x08 // Auto-Wake/Sleep and Portrait/Landscape samples per seconds and Debounce Filter
#define PDET     0x09 // Tap Detection
#define PD       0x0A // Tap Debounce Count

#define MMA7660_RANGE_1_5G 1.5f
#define MMA7660_RANGE_6G   6.0f

#define MMA7660_ACTIVE_MODE 0x01
#define MMA7660_SAMPLE_RATE_32HZ 0x03 // Configured for 32 samples per second (optional)

#define TEST_MODE 0



void sensorDetect(void);
void mma7660_init(I2C_HandleTypeDef *hi2c);
uint8_t mma7660_ReadRegister(I2C_HandleTypeDef *hi2c, uint8_t reg);
void mma7660_set_sample_rate(I2C_HandleTypeDef *hi2c, uint8_t rate);
void mma7660_ReadAcceleration(I2C_HandleTypeDef *hi2c,  float *x,  float *y,  float *z);
HAL_StatusTypeDef mma7660_WriteRegister(I2C_HandleTypeDef *hi2c,uint8_t reg, uint8_t data);
void mma7660_SetResolution(float range_g);

#endif /* MMA7660FC_H_ */
