#ifndef MFRC522_H
#define MFRC522_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h" // HAL library for GPIO and SPI
#include <stdbool.h>       // Boolean type support
#include <stdint.h>        // Fixed-width integers

// MFRC522 Constants
// Page 0: Command and status
#define CommandReg      (0x01 << 1)  // Starts and stops command execution
#define ComIEnReg       (0x02 << 1)  // Enable and disable interrupt request control bits
#define DivIEnReg       (0x03 << 1)  // Enable and disable interrupt request control bits
#define ComIrqReg       (0x04 << 1)  // Interrupt request bits
#define DivIrqReg       (0x05 << 1)  // Interrupt request bits
#define ErrorReg        (0x06 << 1)  // Error bits showing the error status of the last command executed
#define Status1Reg      (0x07 << 1)  // Communication status bits
#define Status2Reg      (0x08 << 1)  // Receiver and transmitter status bits
#define FIFODataReg     (0x09 << 1)  // Input and output of 64-byte FIFO buffer
#define FIFOLevelReg    (0x0A << 1)  // Number of bytes stored in the FIFO buffer
#define WaterLevelReg   (0x0B << 1)  // Level for FIFO underflow and overflow warning
#define ControlReg      (0x0C << 1)  // Miscellaneous control registers
#define BitFramingReg   (0x0D << 1)  // Adjustments for bit-oriented frames
#define CollReg         (0x0E << 1)  // Bit position of the first bit-collision detected on the RF interface

// Page 1: Command
#define ModeReg         (0x11 << 1)  // Defines general modes for transmitting and receiving
#define TxModeReg       (0x12 << 1)  // Defines transmission data rate and framing
#define RxModeReg       (0x13 << 1)  // Defines reception data rate and framing
#define TxControlReg    (0x14 << 1)  // Controls the logical behavior of the antenna driver pins TX1 and TX2
#define TxASKReg        (0x15 << 1)  // Controls the setting of the transmission modulation
#define TxSelReg        (0x16 << 1)  // Selects the internal sources for the antenna driver
#define RxSelReg        (0x17 << 1)  // Selects internal receiver settings
#define RxThresholdReg  (0x18 << 1)  // Selects thresholds for the bit decoder
#define DemodReg        (0x19 << 1)  // Defines demodulator settings
#define MfTxReg         (0x1C << 1)  // Controls some MIFARE communication transmit parameters
#define MfRxReg         (0x1D << 1)  // Controls some MIFARE communication receive parameters
#define SerialSpeedReg  (0x1F << 1)  // Selects the speed of the serial UART interface

// Page 2: Configuration
#define CRCResultRegH   (0x21 << 1)  // Shows the MSB of the CRC calculation
#define CRCResultRegL   (0x22 << 1)  // Shows the LSB of the CRC calculation
#define ModWidthReg     (0x24 << 1)  // Controls the ModWidth setting
#define RFCfgReg        (0x26 << 1)  // Configures the receiver gain
#define GsNReg          (0x27 << 1)  // Selects the conductance of the antenna driver pins TX1 and TX2 for modulation
#define CWGsPReg        (0x28 << 1)  // Defines the conductance of the p-driver output during periods of no modulation
#define ModGsPReg       (0x29 << 1)  // Defines the conductance of the p-driver output during periods of modulation
#define TModeReg        (0x2A << 1)  // Defines settings for the internal timer
#define TPrescalerReg   (0x2B << 1)  // Lower 8 bits of the TPrescaler value
#define TReloadRegH     (0x2C << 1)  // High byte of the 16-bit timer reload value
#define TReloadRegL     (0x2D << 1)  // Low byte of the 16-bit timer reload value
#define TCntValueRegH   (0x2E << 1)  // Shows the high byte of the 16-bit timer value
#define TCntValueRegL   (0x2F << 1)  // Shows the low byte of the 16-bit timer value

// Page 3: Test Registers
#define TestSel1Reg     (0x31 << 1)  // General test signal configuration
#define TestSel2Reg     (0x32 << 1)  // General test signal configuration
#define TestPinEnReg    (0x33 << 1)  // Enables pin output driver on pins D1 to D7
#define TestPinValueReg (0x34 << 1)  // Defines the values for D1 to D7 when it is used as an I/O bus
#define TestBusReg      (0x35 << 1)  // Shows the status of the internal test bus
#define AutoTestReg     (0x36 << 1)  // Controls the digital self-test
#define VersionReg      (0x37 << 1)  // Shows the software version
#define AnalogTestReg   (0x38 << 1)  // Controls the pins AUX1 and AUX2
#define TestDAC1Reg     (0x39 << 1)  // Defines the test value for TestDAC1
#define TestDAC2Reg     (0x3A << 1)  // Defines the test value for TestDAC2
#define TestADCReg      (0x3B << 1)  // Shows the value of ADC I and Q channels

// MFRC522 commands
#define PCD_Idle               0x00  // No action, cancels current command execution
#define PCD_Mem                0x01  // Stores 25 bytes into the internal buffer
#define PCD_GenerateRandomID   0x02  // Generates a 10-byte random ID number
#define PCD_CalcCRC            0x03  // Activates the CRC coprocessor or performs a self-test
#define PCD_Transmit           0x04  // Transmits data from the FIFO buffer
#define PCD_NoCmdChange        0x07  // No command change, modifies CommandReg bits without affecting the command
#define PCD_Receive            0x08  // Activates the receiver circuits
#define PCD_Transceive         0x0C  // Transmits data from FIFO and activates receiver after transmission
#define PCD_MFAuthent          0x0E  // Performs MIFARE authentication
#define PCD_SoftReset          0x0F  // Resets the MFRC522

// PICC commands
#define PICC_CMD_REQA          0x26  // REQuest command, Type A
#define PICC_CMD_WUPA          0x52  // Wake-UP command, Type A
#define PICC_CMD_CT            0x88  // Cascade Tag
#define PICC_CMD_SEL_CL1       0x93  // Anti-collision/Select, Cascade Level 1
#define PICC_CMD_SEL_CL2       0x95  // Anti-collision/Select, Cascade Level 2
#define PICC_CMD_SEL_CL3       0x97  // Anti-collision/Select, Cascade Level 3
#define PICC_CMD_HLTA          0x50  // Halt command, Type A
#define PICC_CMD_MF_AUTH_KEY_A 0x60  // Authenticate with Key A
#define PICC_CMD_MF_AUTH_KEY_B 0x61  // Authenticate with Key B
#define PICC_CMD_MF_READ       0x30  // Read a block
#define PICC_CMD_MF_WRITE      0xA0  // Write a block
#define PICC_CMD_MF_DECREMENT  0xC0  // Decrement a block value
#define PICC_CMD_MF_INCREMENT  0xC1  // Increment a block value
#define PICC_CMD_MF_RESTORE    0xC2  // Restore a block
#define PICC_CMD_MF_TRANSFER   0xB0  // Transfer a block
#define PICC_CMD_UL_WRITE      0xA2  // Write a 4-byte page

// MIFARE constants
#define MF_ACK                 0xA   // Acknowledge value for MIFARE Classic
#define MF_KEY_SIZE            6     // MIFARE Crypto1 key size in bytes

// PICC types
#define PICC_TYPE_UNKNOWN      0      // Unknown PICC type
#define PICC_TYPE_ISO_14443_4  1      // PICC compliant with ISO/IEC 14443-4
#define PICC_TYPE_ISO_18092    2      // PICC compliant with ISO/IEC 18092 (NFC)
#define PICC_TYPE_MIFARE_MINI  3      // MIFARE Classic Mini (320 bytes)
#define PICC_TYPE_MIFARE_1K    4      // MIFARE Classic 1K
#define PICC_TYPE_MIFARE_4K    5      // MIFARE Classic 4K
#define PICC_TYPE_MIFARE_UL    6      // MIFARE Ultralight
#define PICC_TYPE_MIFARE_PLUS  7      // MIFARE Plus
#define PICC_TYPE_TNP3XXX      8      // TNP3XXX
#define PICC_TYPE_NOT_COMPLETE 255    // UID is not complete

// Status codes
#define STATUS_OK              1  // Success
#define STATUS_ERROR           2  // Error in communication
#define STATUS_COLLISION       3  // Collision detected
#define STATUS_TIMEOUT         4  // Timeout in communication
#define STATUS_NO_ROOM         5  // Buffer is not large enough
#define STATUS_INTERNAL_ERROR  6  // Internal error
#define STATUS_INVALID         7  // Invalid argument
#define STATUS_CRC_WRONG       8  // CRC does not match
#define STATUS_MIFARE_NACK     9  // MIFARE PICC responded with NAK

#define MFRC522_MaxPICCs (sizeof(_TypeNamePICC)/sizeof(_TypeNamePICC[0]))
#define MFRC522_MaxError (sizeof(_ErrorMessage)/sizeof(_ErrorMessage[0]))


// Structs
typedef struct {
    uint8_t size;        // Number of bytes in the UID
    uint8_t uidByte[10]; // UID bytes
    uint8_t sak;         // Select Acknowledge (SAK) byte
} Uid;

extern Uid uid;

typedef struct {
    uint8_t keyByte[6];  // MIFARE Crypto1 key
} MIFARE_Key;

typedef struct {
    uint8_t *validBits;  // Optional: Valid bits
    uint8_t rxAlign;     // Optional: Receive alignment
    bool checkCRC;       // Optional: Whether to check CRC
} TransceiveConfig;

// GPIO Pins
#define RC522_CS_GPIO_Port GPIOB
#define RC522_CS_Pin GPIO_PIN_6
#define RC522_RESET_GPIO_Port GPIOC
#define RC522_RESET_Pin GPIO_PIN_7

// Function Prototypes

// Initialization and Configuration
void PCD_Init(void);
void PCD_Reset(void);
void PCD_AntennaOn(void);

// Low-Level Register Access
void PCD_WriteRegister(uint8_t reg, uint8_t value);
void PCD_WriteRegisterMulti(uint8_t reg, uint8_t count, uint8_t *values);
uint8_t PCD_ReadRegister(uint8_t reg);
void PCD_ReadRegisterMulti(uint8_t reg, uint8_t count, uint8_t *values);
void PCD_SetRegisterBits(uint8_t reg, uint8_t mask);
void PCD_ClrRegisterBits(uint8_t reg, uint8_t mask);

// Communication with the MFRC522
uint8_t PCD_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen,
                                uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, bool checkCRC);
PCD_TransceiveData2(uint8_t *sendData,
                                    uint8_t sendLen,
                                    uint8_t *backData,
                                    uint8_t *backLen,
                                    uint8_t *validBits,
                                    uint8_t rxAlign,
                                    bool    checkCRC);

uint8_t PCD_TransceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t validBits);
uint8_t PCD_MIFARE_Transceive(uint8_t *sendData, uint8_t sendLen, bool acceptTimeout);

uint8_t PCD_TransceiveDataDefault(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen);
uint8_t PCD_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result);
uint8_t PICC_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize);

// PICC-Level Commands
uint8_t PICC_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize);
uint8_t PICC_WakeupA(uint8_t *bufferATQA, uint8_t *bufferSize);
uint8_t PICC_Select(Uid *uid, uint8_t validBits);
uint8_t PICC_HaltA(void);

// MIFARE Commands
uint8_t PCD_Authenticate(uint8_t command, uint8_t blockAddr, MIFARE_Key *key, Uid *uid);
void PCD_StopCrypto1(void);
uint8_t MIFARE_Read(uint8_t blockAddr, uint8_t *buffer, uint8_t *bufferSize);
uint8_t MIFARE_Write(uint8_t blockAddr, uint8_t *buffer, uint8_t bufferSize);
uint8_t MIFARE_UltralightWrite(uint8_t page, uint8_t *buffer, uint8_t bufferSize);
uint8_t MIFARE_Decrement(uint8_t blockAddr, uint32_t delta);
uint8_t MIFARE_Increment(uint8_t blockAddr, uint32_t delta);
uint8_t MIFARE_Restore(uint8_t blockAddr);
uint8_t MIFARE_Transfer(uint8_t blockAddr);

// Utilities
uint8_t PICC_GetType(uint8_t sak);
char *PICC_GetTypeName(uint8_t type);
char *GetStatusCodeName(uint8_t code);
void MIFARE_SetAccessBits(uint8_t *accessBitBuffer, uint8_t g0, uint8_t g1, uint8_t g2, uint8_t g3);
bool PICC_IsNewCardPresent(void);
bool PICC_ReadCardSerial(void);

uint8_t MIFARE_TwoStepHelper(uint8_t command, uint8_t blockAddr, uint32_t data);

#ifdef __cplusplus
}
#endif

#endif // MFRC522_H
