#ifndef MFRC522_H
#define MFRC522_H

// Include Libraries
#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>


// MFRC522 registers
#define CommandReg             (0x01 << 1)
#define ComIEnReg              (0x02 << 1)
#define DivIEnReg              (0x03 << 1)
#define ComIrqReg              (0x04 << 1)
#define DivIrqReg              (0x05 << 1)
#define ErrorReg               (0x06 << 1)
#define Status1Reg             (0x07 << 1)
#define Status2Reg             (0x08 << 1)
#define FIFODataReg            (0x09 << 1)
#define FIFOLevelReg           (0x0A << 1)
#define WaterLevelReg          (0x0B << 1)
#define ControlReg             (0x0C << 1)
#define BitFramingReg          (0x0D << 1)
#define CollReg                (0x0E << 1)

// Page 1: Command
#define ModeReg                (0x11 << 1)
#define TxModeReg              (0x12 << 1)
#define RxModeReg              (0x13 << 1)
#define TxControlReg           (0x14 << 1)
#define TxASKReg               (0x15 << 1)
#define TxSelReg               (0x16 << 1)
#define RxSelReg               (0x17 << 1)
#define RxThresholdReg         (0x18 << 1)
#define DemodReg               (0x19 << 1)
#define MfTxReg                (0x1C << 1)
#define MfRxReg                (0x1D << 1)
#define SerialSpeedReg         (0x1F << 1)

// Page 2: Configuration
#define CRCResultRegH          (0x21 << 1)
#define CRCResultRegL          (0x22 << 1)
#define ModWidthReg            (0x24 << 1)
#define RFCfgReg               (0x26 << 1)
#define GsNReg                 (0x27 << 1)
#define CWGsPReg               (0x28 << 1)
#define ModGsPReg              (0x29 << 1)
#define TModeReg               (0x2A << 1)
#define TPrescalerReg          (0x2B << 1)
#define TReloadRegH            (0x2C << 1)
#define TReloadRegL            (0x2D << 1)
#define TCntValueRegH          (0x2E << 1)
#define TCntValueRegL          (0x2F << 1)

// Page 3: Test Registers
#define TestSel1Reg            (0x31 << 1)
#define TestSel2Reg            (0x32 << 1)
#define TestPinEnReg           (0x33 << 1)
#define TestPinValueReg        (0x34 << 1)
#define TestBusReg             (0x35 << 1)
#define AutoTestReg            (0x36 << 1)
#define VersionReg             (0x37 << 1)
#define AnalogTestReg          (0x38 << 1)
#define TestDAC1Reg            (0x39 << 1)
#define TestDAC2Reg            (0x3A << 1)
#define TestADCReg             (0x3B << 1)

// MFRC522 commands
#define PCD_Idle               0x00
#define PCD_Mem                0x01
#define PCD_GenerateRandomID   0x02
#define PCD_CalcCRC            0x03
#define PCD_Transmit           0x04
#define PCD_NoCmdChange        0x07
#define PCD_Receive            0x08
#define PCD_Transceive         0x0C
#define PCD_MFAuthent          0x0E
#define PCD_SoftReset          0x0F

// PICC Commands
#define PICC_CMD_REQA          0x26
#define PICC_CMD_WUPA          0x52
#define PICC_CMD_CT            0x88
#define PICC_CMD_SEL_CL1       0x93
#define PICC_CMD_SEL_CL2       0x95
#define PICC_CMD_SEL_CL3       0x97
#define PICC_CMD_HLTA          0x50
#define PICC_CMD_MF_AUTH_KEY_A 0x60
#define PICC_CMD_MF_AUTH_KEY_B 0x61
#define PICC_CMD_MF_READ       0x30
#define PICC_CMD_MF_WRITE      0xA0
#define PICC_CMD_MF_DECREMENT  0xC0
#define PICC_CMD_MF_INCREMENT  0xC1
#define PICC_CMD_MF_RESTORE    0xC2
#define PICC_CMD_MF_TRANSFER   0xB0
#define PICC_CMD_UL_WRITE      0xA2

// MIFARE constants
#define MF_ACK                 0xA
#define MF_KEY_SIZE            6

// PICC Types
#define PICC_TYPE_UNKNOWN      0
#define PICC_TYPE_ISO_14443_4  1
#define PICC_TYPE_ISO_18092    2
#define PICC_TYPE_MIFARE_MINI  3
#define PICC_TYPE_MIFARE_1K    4
#define PICC_TYPE_MIFARE_4K    5
#define PICC_TYPE_MIFARE_UL    6
#define PICC_TYPE_MIFARE_PLUS  7
#define PICC_TYPE_TNP3XXX      8
#define PICC_TYPE_NOT_COMPLETE 255

// Status codes
#define STATUS_OK              1
#define STATUS_ERROR           2
#define STATUS_COLLISION       3
#define STATUS_TIMEOUT         4
#define STATUS_NO_ROOM         5
#define STATUS_INTERNAL_ERROR  6
#define STATUS_INVALID         7
#define STATUS_CRC_WRONG       8
#define STATUS_MIFARE_NACK     9


// constants
static const uint8_t FIFO_SIZE = 64;   // The FIFO is 64 bytes.
#define MF_KEY_SIZE 6


// Structures
typedef struct {
	uint8_t size;               // Number of bytes in the UID: 4, 7, or 10.
	uint8_t uidByte[10];
	uint8_t sak;                // The SAK (Select Acknowledge) byte returned from the PICC after successful selection.
} Uid;

typedef struct {
	uint8_t keyByte[MF_KEY_SIZE];
} MIFARE_Key;


// Functions for manipulating the MFRC522
void PCD_Init(void);
void PCD_Reset(void);
void PCD_AntennaOn(void);
void PCD_WriteRegister(uint8_t reg, uint8_t value);
void PCD_WriteRegisterMulti(uint8_t reg, uint8_t count, const uint8_t *values);
uint8_t PCD_ReadRegister(uint8_t reg);
void PCD_ReadRegisterMulti(uint8_t reg, uint8_t count, uint8_t *values);
void PCD_ClearFIFO(void);
void PCD_SetRegisterBits(uint8_t reg, uint8_t mask);
void PCD_ClrRegisterBits(uint8_t reg, uint8_t mask);
uint8_t PCD_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result);
uint8_t PCD_TransceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, bool checkCRC);
uint8_t PCD_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, bool checkCRC);

// Functions for communicating with PICCs
uint8_t PICC_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize);
uint8_t PICC_WakeupA(uint8_t *bufferATQA, uint8_t *bufferSize);
uint8_t PICC_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize);
uint8_t PICC_Select(Uid *uid, uint8_t validBits);
uint8_t PICC_HaltA(void);

// Functions for MIFARE authentication and data manipulation
uint8_t PCD_Authenticate(uint8_t command, uint8_t blockAddr, MIFARE_Key *key, Uid *uid);
void PCD_StopCrypto1(void);
uint8_t MIFARE_Read(uint8_t blockAddr, uint8_t *buffer, uint8_t *bufferSize);
uint8_t MIFARE_Write(uint8_t blockAddr, uint8_t *buffer, uint8_t bufferSize);
uint8_t MIFARE_UltralightWrite(uint8_t page, uint8_t *buffer, uint8_t bufferSize);
uint8_t MIFARE_Decrement(uint8_t blockAddr, uint32_t delta);
uint8_t MIFARE_Increment(uint8_t blockAddr, uint32_t delta);
uint8_t MIFARE_Restore(uint8_t blockAddr);
uint8_t MIFARE_Transfer(uint8_t blockAddr);

// Support and utility functions
uint8_t PCD_MIFARE_Transceive(uint8_t *sendData, uint8_t sendLen, bool acceptTimeout);
uint8_t PICC_GetType(uint8_t sak);
char *PICC_GetTypeName(uint8_t type);
char *GetStatusCodeName(uint8_t code);
void MIFARE_SetAccessBits(uint8_t *accessBitBuffer, uint8_t g0, uint8_t g1, uint8_t g2, uint8_t g3);
bool PICC_IsNewCardPresent(void);
bool PICC_ReadCardSerial(Uid *uid);
uint8_t MIFARE_TwoStepHelper(uint8_t command, uint8_t blockAddr, uint32_t data);

#endif // MFRC522_H

