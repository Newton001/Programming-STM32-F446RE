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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdio.h"
#include "lcd_driver.h"
#include "MFRC522_Driver.h"
#include <string.h>

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int _write(int fd, char*file, int len);
void lcd_initialize(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t readValue;
void HAL_ReadCardUIDAndReadBlocks();
void HAL_WriteToBlocks();

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
	MX_SPI1_Init();
	MX_SPI2_Init();
	/* USER CODE BEGIN 2 */

	// Initialize the LCD
	lcd_initialize();

	//Initialize PCD
	PCD_Init();
	printf("Checking for cards...\n");
	HAL_ReadCardUIDAndReadBlocks();
	HAL_WriteToBlocks();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		// Write a value to the register

		HAL_Delay(1000);

		/* USER CODE END WHILE */


		/* USER CODE BEGIN 3 */
		//printf("This is how it starts\n\r");
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

// printf Inclusion on Screen for Debugging
int _write(int fd, char*ptr, int len)
{
	if(HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY)== HAL_OK)
		return len;
	else
		return -1;
}

// Initialize the LCD_Driver in a few lines

void lcd_initialize(void)
{
	lcd_init();
	lcd_clear();
	// Set it to some line
	lcd_setLine(127, 0, 127, 31, 1);
	lcd_setLine(0, 0, 0, 31, 1);
	lcd_setString(4, 3, "System Initialization Test ", LCD_FONT_8, false);
	lcd_setString(4, 15, "I2C connection Done", LCD_FONT_8, false);
	lcd_setString(4, 25, "Accelerometer connection Done", LCD_FONT_8, false);
	lcd_show();
	HAL_Delay(2500);
	lcd_clear();

	puts("******** LETS PRACTICE ******** \r\n");
	puts("- Uart connection ... Done\r\n");
	puts("- printf retargeting to uart ... Done\r\n");
	puts("- Temperature Sensor connection ... Done\r\n");
	puts("***************************** \r\n");
}

// Test the Registers

void HAL_ReadCardUIDAndReadBlocks() {
    Uid uid;
    MIFARE_Key key = { {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} }; // Default key
    uint8_t buffer[18];
    uint8_t bufferSize = sizeof(buffer);
    uint8_t status;

    printf("Starting RFID card detection and block reading...\n");

    // Check for a new card
    if (!PICC_IsNewCardPresent()) {
        printf("No new card detected.\n");
        return;
    }

    if (!PICC_ReadCardSerial(&uid)) {
        printf("Failed to read card serial.\n");
        return;
    }

    // Output UID
    printf("Card UID: ");
    for (uint8_t i = 0; i < uid.size; i++) {
        printf("%02X ", uid.uidByte[i]);
    }
    printf("\n");

    // Authenticate and read block 4
    printf("Authenticating block 4...\n");
    status = PCD_Authenticate(PICC_CMD_MF_AUTH_KEY_A, 4, &key, &uid);
    if (status != STATUS_OK) {
        printf("Authentication failed for block 4: %s\n", GetStatusCodeName(status));
        PCD_StopCrypto1();
        return;
    }

    printf("Reading block 4...\n");
    status = MIFARE_Read(4, buffer, &bufferSize);
    if (status != STATUS_OK) {
        printf("Read failed for block 4: %s\n", GetStatusCodeName(status));
        PCD_StopCrypto1();
        return;
    }

    printf("Block 4 Data: ");
    for (uint8_t i = 0; i < bufferSize; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    // Authenticate and read block 7
    printf("Authenticating block 7...\n");
    status = PCD_Authenticate(PICC_CMD_MF_AUTH_KEY_A, 7, &key, &uid);
    if (status != STATUS_OK) {
        printf("Authentication failed for block 7: %s\n", GetStatusCodeName(status));
        PCD_StopCrypto1();
        return;
    }

    printf("Reading block 7...\n");
    bufferSize = sizeof(buffer); // Reset buffer size
    status = MIFARE_Read(7, buffer, &bufferSize);
    if (status != STATUS_OK) {
        printf("Read failed for block 7: %s\n", GetStatusCodeName(status));
        PCD_StopCrypto1();
        return;
    }

    printf("Block 7 Data: ");
    for (uint8_t i = 0; i < bufferSize; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    // Stop crypto operations
    PCD_StopCrypto1();
    printf("Finished reading blocks 4 and 7.\n");
}


void HAL_WriteToBlocks() {
    Uid uid;
    MIFARE_Key key = { {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} }; // Default key
    uint8_t writeBufferBlock5[16] = {
            'S', 'e', 's', 's', 'i', 'o', 'n', 's',
            ' ', '2', ' ', ' ', ' ', ' ', ' ', ' '
    }; // Data for block 5: "Sessions 2"
    uint8_t status;

    printf("Starting RFID card detection and block writing...");

    // Check for a new card
    if (!PICC_IsNewCardPresent()) {
        printf("No new card detected.");
        return;
    }

    if (!PICC_ReadCardSerial(&uid)) {
        printf("Failed to read card serial.");
        return;
    }

    // Output UID
    printf("Card UID: ");
    for (uint8_t i = 0; i < uid.size; i++) {
        printf("%02X ", uid.uidByte[i]);
    }
    printf("\n");

    // Authenticate and write to block 7
    printf("Authenticating block 7...");
    status = PCD_Authenticate(PICC_CMD_MF_AUTH_KEY_A, 7, &key, &uid);
    if (status != STATUS_OK) {
        printf("Authentication failed for block 7: %s", GetStatusCodeName(status));
        PCD_StopCrypto1();
        return;
    }

    printf("Writing to block 7...");
    status = MIFARE_Write(7, writeBufferBlock5, sizeof(writeBufferBlock5));
    if (status != STATUS_OK) {
        printf("Write failed for block 7: %s", GetStatusCodeName(status));
        PCD_StopCrypto1();
        return;
    }

    printf("Data written successfully to block 7.");

    // Stop crypto operations
    PCD_StopCrypto1();
    printf("Finished writing to block 7.");
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
