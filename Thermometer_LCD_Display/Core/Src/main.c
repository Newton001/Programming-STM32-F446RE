/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "lcd_driver.h"
#include "stdio.h"
#include "thermo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int doUpdate = 0;
int yPosition = 4;                  // initial vertical position
int xPosition = 4;                  // initial Horizontal position
int direction = 1;                  // direction of movement (1 = down, -1 = up)

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void displayWelcome(void);
int _write(int fd, char *ptr, int len);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_TIM7_Init();
	/* USER CODE BEGIN 2 */

	displayWelcome();

	// Initialize LCD
	lcd_init();
	lcd_clear();

	// Set it to some line
	lcd_setLine(127,0,127,31,1);
	lcd_setLine(0,0,0,31,1);
	lcd_setString(4,3,"All Ladies say", LCD_FONT_8,false);
	lcd_setString(4,15,"'WE LOVE ❤️ ' ",LCD_FONT_8,false);
	lcd_setString(4,25," THE  K - man!  ",LCD_FONT_8,false);
	lcd_show();
	HAL_Delay(2500);
	lcd_clear();

	// Start the Timer
	HAL_TIM_Base_Start_IT(&htim7); // We wanna read the temp every 200ms


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	char msgBuf[30]; // Preparing Buffer for the message
	while (1)
	{
//		if(doUpdate)
//		{
//			doUpdate = 0; 				// reset update flag
//			float tmp = readTemp(); 	// read actual temperature
//			sprintf(msgBuf,"Temperature: %2.1f%cC",tmp,0x80); // fill message buffer
//			lcd_setString(4,4, msgBuf,LCD_FONT_8, false); // lcd message
//			lcd_show();
//			puts(msgBuf); // Uart message
//			puts("\r");
//		}

		if (doUpdate) {
		        doUpdate = 0;               // reset update flag
		        float tmp = readTemp();     // read actual temperature
		        sprintf(msgBuf, "Temperature: %2.2f %cC", tmp, 0x80); // fill message buffer
		        lcd_clear();

		        // Display temperature at current y-position
		        lcd_setString(xPosition, yPosition, msgBuf, LCD_FONT_8, false);
		        lcd_show();
		        puts(msgBuf);               // Uart message
		        puts("\r");

		        // Adjust y-position for smooth gliding effect
		        yPosition += direction;     // move position up or down

		        // Reverse direction if boundaries are reached
		        if (yPosition >= 16 || yPosition <= 4) {
		            direction *= -1;        // change direction
		        }

		        HAL_Delay(50); // delay to control glide speed
		    }
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
int _write(int fd, char *ptr, int len)
{
	if(HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY)== HAL_OK)
		return len;
	else
		return -1;
}

void displayWelcome()
{
	puts("******** LETS PRACTICE ******** \r\n");
	puts("- Uart connection ... Done\r\n");
	puts("- printf retargeting to uart ... Done\r\n");
	puts("- Temperature Sensor connection ... Done\r\n");
	puts("***************************** \r\n");
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	doUpdate = 1;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
