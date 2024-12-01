/*
 * mma7660fc.h
 *
 *  Created on: Oct 29, 2024
 *      Author: newton
 */

#ifndef MMA7660FC_H_
#define MMA7660FC_H_
#include "stdint.h"

void sensorDetect(void);
void mma7660_init(void);

float mma7660_ReadRegister(I2C_HandleTypeDef *hi2c, uint8_t reg);
void mma7660_ReadAcceleration(I2C_HandleTypeDef *hi2c, uint8_t *x, uint8_t *y, uint8_t *z);
HAL_StatusTypeDef mma7660_WriteRegister(uint8_t reg, uint8_t data);

#endif /* MMA7660FC_H_ */
