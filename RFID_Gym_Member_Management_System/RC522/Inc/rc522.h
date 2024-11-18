/*
 * rc522.h
 *
 * Created on: Nov 3, 2024
 * Author: Newton
 */

#ifndef RC522_H_
#define RC522_H_

#include "stm32f4xx_hal.h"
#include "spi.h"

// Define the SPI handler (defined in the main application file)
extern SPI_HandleTypeDef hspi1;

// MFRC522 buffer size
#define MAX_LEN 16

// MFRC522 Command Set
#define PCD_IDLE              0x00   // No action; cancel the current command
#define PCD_AUTHENT           0x0E   // Authenticate with Key A or B
#define PCD_RECEIVE           0x08   // Receive data
#define PCD_TRANSMIT          0x04   // Transmit data
#define PCD_TRANSCEIVE        0x0C   // Transmit and receive data
#define PCD_RESETPHASE        0x0F   // Reset the MFRC522
#define PCD_CALCCRC           0x03   // Calculate CRC

// PICC Commands for MIFARE_One cards
#define PICC_REQIDL           0x26   // Request idle mode
#define PICC_REQALL           0x52   // Request all cards
#define PICC_ANTICOLL         0x93   // Anti-collision command
#define PICC_SELECTTAG        0x93   // Select a card
#define PICC_AUTHENT1A        0x60   // Authenticate with Key A
#define PICC_AUTHENT1B        0x61   // Authenticate with Key B
#define PICC_READ             0x30   // Read block
#define PICC_WRITE            0xA0   // Write block
#define PICC_DECREMENT        0xC0   // Decrement value block
#define PICC_INCREMENT        0xC1   // Increment value block
#define PICC_RESTORE          0xC2   // Restore block data
#define PICC_TRANSFER         0xB0   // Transfer block data
#define PICC_HALT             0x50   // Put the card into halt state

// MFRC522 Registers
// Page 0: Command and Status
#define Reserved00            0x00
#define CommandReg            (0x01 << 1)  // Starts and stops command execution
#define ComIEnReg             (0x02 << 1)  // Enable and disable interrupt request control bits
#define DivIEnReg             (0x03 << 1)  // Enable and disable interrupt request control bits
#define ComIrqReg             (0x04 << 1)  // Interrupt request bits
#define DivIrqReg             (0x05 << 1)  // Interrupt request bits
#define ErrorReg              (0x06 << 1)  // Error bits showing the error status of the last command executed
#define Status1Reg            (0x07 << 1)  // Communication status bits
#define Status2Reg            (0x08 << 1)  // Receiver and transmitter status bits
#define FIFODataReg           (0x09 << 1)  // Input and output of 64 byte FIFO buffer
#define FIFOLevelReg          (0x0A << 1)  // Number of bytes stored in the FIFO buffer
#define WaterLevelReg         (0x0B << 1)  // Level for FIFO underflow and overflow warning
#define ControlReg            (0x0C << 1)  // Miscellaneous control registers
#define BitFramingReg         (0x0D << 1)  // Adjustments for bit-oriented frames
#define CollReg               (0x0E << 1)  // Bit position of the first bit-collision detected on the RF interface

// Page 1: Command
#define ModeReg               (0x11 << 1)  // Defines general modes for transmitting and receiving
#define TxModeReg             (0x12 << 1)  // Defines transmission data rate and framing
#define RxModeReg             (0x13 << 1)  // Defines reception data rate and framing
#define TxControlReg          (0x14 << 1)  // Controls the logical behavior of the antenna driver pins TX1 and TX2
#define TxASKReg              (0x15 << 1)  // Controls the setting of the transmission modulation
#define TxSelReg              (0x16 << 1)  // Selects the internal sources for the antenna driver
#define RxSelReg              (0x17 << 1)  // Selects internal receiver settings
#define RxThresholdReg        (0x18 << 1)  // Selects thresholds for the bit decoder
#define DemodReg              (0x19 << 1)  // Defines demodulator settings

// Page 2: Configuration
#define CRCResultRegH         (0x21 << 1)  // Shows the MSB and LSB values of the CRC calculation
#define CRCResultRegL         (0x22 << 1)
#define ModWidthReg           (0x24 << 1)  // Controls the ModWidth setting
#define RFCfgReg              (0x26 << 1)  // Configures the receiver gain
#define GsNReg                (0x27 << 1)  // Selects the conductance of the antenna driver pins TX1 and TX2 for modulation
#define CWGsPReg              (0x28 << 1)  // Defines the conductance of the p-driver output during periods of no modulation
#define ModGsPReg             (0x29 << 1)  // Defines the conductance of the p-driver output during periods of modulation
#define TModeReg              (0x2A << 1)  // Defines settings for the internal timer
#define TPrescalerReg         (0x2B << 1)  // The lower 8 bits of the TPrescaler value. The 4 high bits are in TModeReg
#define TReloadRegH           (0x2C << 1)  // Defines the 16-bit timer reload value
#define TReloadRegL           (0x2D << 1)
#define TCounterValueRegH     (0x2E << 1)  // Shows the 16-bit timer value
#define TCounterValueRegL     (0x2F << 1)

// Page 3: Test Registers
#define TestSel1Reg           (0x31 << 1)  // General test signal configuration
#define TestSel2Reg           (0x32 << 1)  // General test signal configuration
#define TestPinEnReg          (0x33 << 1)  // Enables pin output driver on pins D1 to D7
#define TestPinValueReg       (0x34 << 1)  // Defines the values for D1 to D7 when it is used as an I/O bus
#define TestBusReg            (0x35 << 1)  // Shows the status of the internal test bus
#define AutoTestReg           (0x36 << 1)  // Controls the digital self-test
#define VersionReg            (0x37 << 1)  // Shows the software version
#define AnalogTestReg         (0x38 << 1)  // Controls the pins AUX1 and AUX2
#define TestDAC1Reg           (0x39 << 1)  // Defines the test value for TestDAC1
#define TestDAC2Reg           (0x3A << 1)  // Defines the test value for TestDAC2
#define TestADCReg            (0x3B << 1)  // Shows the value of ADC I and Q channels

// Status Codes
#define MI_OK                 0   // Success
#define MI_NOTAGERR           1   // No tag error
#define MI_ERR                2   // General error
#define MI_TIMEOUT 			  3

// Define the RC522 handle structure
typedef struct {
    SPI_HandleTypeDef *hspi;      // SPI handler (e.g., &hspi1)
    GPIO_TypeDef *csPort;         // CS port (e.g., GPIOA)
    uint16_t csPin;               // CS pin (e.g., GPIO_PIN_4)
    GPIO_TypeDef *rstPort;        // RST port (e.g., GPIOA)
    uint16_t rstPin;              // RST pin (e.g., GPIO_PIN_5)
} RC522_HandleTypeDef;

// Function prototypes (replace `SPI_HandleTypeDef` with `RC522_HandleTypeDef *rc522`)
void RC522_Init(RC522_HandleTypeDef *rc522, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csPort, uint16_t csPin, GPIO_TypeDef *rstPort, uint16_t rstPin);
;
void RC522_CalculateCRC(RC522_HandleTypeDef *rc522, uint8_t *data, uint8_t length, uint8_t *result);
void RC522_Reset(RC522_HandleTypeDef *rc522);
uint8_t RC522_Request(RC522_HandleTypeDef *rc522, uint8_t reqMode, uint8_t *TagType);
uint8_t RC522_Anticoll(RC522_HandleTypeDef *rc522, uint8_t *serNum);
uint8_t RC522_SelectTag(RC522_HandleTypeDef *rc522, uint8_t *serNum);
uint8_t RC522_Auth(RC522_HandleTypeDef *rc522, uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum);
uint8_t RC522_Write(RC522_HandleTypeDef *rc522, uint8_t blockAddr, uint8_t *writeData);
uint8_t RC522_Read(RC522_HandleTypeDef *rc522, uint8_t blockAddr, uint8_t *recvData);
void RC522_Halt(RC522_HandleTypeDef *rc522);
uint8_t RC522_ToCard(RC522_HandleTypeDef *rc522, uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen);

// Utility functions for register manipulation
void Write_RC522(RC522_HandleTypeDef *rc522, uint8_t reg, uint8_t value);
uint8_t Read_RC522(RC522_HandleTypeDef *rc522, uint8_t reg);
void SetBitMask(RC522_HandleTypeDef *rc522, uint8_t reg, uint8_t mask);
void ClearBitMask(RC522_HandleTypeDef *rc522, uint8_t reg, uint8_t mask);
void antenna_on(RC522_HandleTypeDef *rc522);
void antenna_off(RC522_HandleTypeDef *rc522);
void RC522_WriteRegister(RC522_HandleTypeDef *rc522, uint8_t reg, uint8_t value);
void RC522_SetRegisterBits(RC522_HandleTypeDef *rc522, uint8_t reg, uint8_t mask);


#endif /* RC522_H_ */
