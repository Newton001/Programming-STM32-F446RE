/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MFRC522_MOSI_Pin GPIO_PIN_1
#define MFRC522_MOSI_GPIO_Port GPIOC
#define MFRC522_MISO_Pin GPIO_PIN_2
#define MFRC522_MISO_GPIO_Port GPIOC
#define LCD_SCK_Pin GPIO_PIN_5
#define LCD_SCK_GPIO_Port GPIOA
#define LCD_RESET_Pin GPIO_PIN_6
#define LCD_RESET_GPIO_Port GPIOA
#define LCD_MOSI_Pin GPIO_PIN_7
#define LCD_MOSI_GPIO_Port GPIOA
#define MFRC522_SCLK_Pin GPIO_PIN_10
#define MFRC522_SCLK_GPIO_Port GPIOB
#define MFRC522_CS_N_Pin GPIO_PIN_12
#define MFRC522_CS_N_GPIO_Port GPIOB
#define LCD_A0_Pin GPIO_PIN_8
#define LCD_A0_GPIO_Port GPIOA
#define MFRC522_RESET_Pin GPIO_PIN_11
#define MFRC522_RESET_GPIO_Port GPIOC
#define LCD_CS_N_Pin GPIO_PIN_6
#define LCD_CS_N_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
