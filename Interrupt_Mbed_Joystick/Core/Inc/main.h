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
#define Right_Pin GPIO_PIN_0
#define Right_GPIO_Port GPIOC
#define Right_EXTI_IRQn EXTI0_IRQn
#define Left_Pin GPIO_PIN_1
#define Left_GPIO_Port GPIOC
#define Left_EXTI_IRQn EXTI1_IRQn
#define Up_Pin GPIO_PIN_4
#define Up_GPIO_Port GPIOA
#define Up_EXTI_IRQn EXTI4_IRQn
#define Green_Pin GPIO_PIN_7
#define Green_GPIO_Port GPIOC
#define Blue_Pin GPIO_PIN_9
#define Blue_GPIO_Port GPIOA
#define Red_Pin GPIO_PIN_4
#define Red_GPIO_Port GPIOB
#define Centre_Pin GPIO_PIN_5
#define Centre_GPIO_Port GPIOB
#define Centre_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
