/*
 * RC522.c - Library for using the RC522 RFID module with STM32 HAL
 * Created on: Nov 12, 2024
 * Author: Newton
 */

#include "RC522.h"
#include <string.h>

// Array of PICC type names
//static const char* _TypeNamePICC[] = {
//    "Unknown type",
//    "PICC compliant with ISO/IEC 14443-4",
//    "PICC compliant with ISO/IEC 18092 (NFC)",
//    "MIFARE Mini, 320 bytes",
//    "MIFARE 1KB",
//    "MIFARE 4KB",
//    "MIFARE Ultralight or Ultralight C",
//    "MIFARE Plus",
//    "MIFARE TNP3XXX",
//    "SAK indicates UID is not complete"
//};
//
//// Array of error messages
//static const char* _ErrorMessage[] = {
//    "Unknown error",
//    "Success",
//    "Error in communication",
//    "Collision detected",
//    "Timeout in communication",
//    "A buffer is not big enough",
//    "Internal error in the code, should not happen",
//    "Invalid argument",
//    "The CRC_A does not match",
//    "A MIFARE PICC responded with NAK"
//};

// Initialize the RC522 module
void RC522_Init(RC522_HandleTypeDef *rc522, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csPort, uint16_t csPin, GPIO_TypeDef *rstPort, uint16_t rstPin) {
    rc522->hspi = hspi;
    rc522->csPort = csPort;
    rc522->csPin = csPin;
    rc522->rstPort = rstPort;
    rc522->rstPin = rstPin;

    // Perform a reset
    HAL_GPIO_WritePin(rc522->rstPort, rc522->rstPin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(rc522->rstPort, rc522->rstPin, GPIO_PIN_SET);
    HAL_Delay(50);

    // Configure initial settings
    RC522_WriteRegister(rc522, TModeReg, 0x80);
    RC522_WriteRegister(rc522, TPrescalerReg, 0xA9);
    RC522_WriteRegister(rc522, TReloadRegH, 0x03);
    RC522_WriteRegister(rc522, TReloadRegL, 0xE8);
    RC522_WriteRegister(rc522, TxASKReg, 0x40);
    RC522_WriteRegister(rc522, ModeReg, 0x3D);
    RC522_SetRegisterBits(rc522, TxControlReg, 0x03);  // Enable antenna
}

// Write a byte to a specified register
void RC522_WriteRegister(RC522_HandleTypeDef *rc522, uint8_t reg, uint8_t value) {
    HAL_GPIO_WritePin(rc522->csPort, rc522->csPin, GPIO_PIN_RESET);
    uint8_t data[] = { (reg << 1) & 0x7E, value };
    HAL_SPI_Transmit(rc522->hspi, data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(rc522->csPort, rc522->csPin, GPIO_PIN_SET);
}

// Read a byte from a specified register
uint8_t RC522_ReadRegister(RC522_HandleTypeDef *rc522, uint8_t reg) {
    HAL_GPIO_WritePin(rc522->csPort, rc522->csPin, GPIO_PIN_RESET);
    uint8_t address = ((reg << 1) & 0x7E) | 0x80;
    uint8_t value;
    HAL_SPI_Transmit(rc522->hspi, &address, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(rc522->hspi, &value, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(rc522->csPort, rc522->csPin, GPIO_PIN_SET);
    return value;
}

// Set specific bits in a register
void RC522_SetRegisterBits(RC522_HandleTypeDef *rc522, uint8_t reg, uint8_t mask) {
    uint8_t tmp = RC522_ReadRegister(rc522, reg);
    RC522_WriteRegister(rc522, reg, tmp | mask);
}

// Clear specific bits in a register
void RC522_ClearRegisterBits(RC522_HandleTypeDef *rc522, uint8_t reg, uint8_t mask) {
    uint8_t tmp = RC522_ReadRegister(rc522, reg);
    RC522_WriteRegister(rc522, reg, tmp & (~mask));
}

// Reset the RC522 module
void RC522_Reset(RC522_HandleTypeDef *rc522) {
    RC522_WriteRegister(rc522, CommandReg, PCD_RESETPHASE);
    HAL_Delay(50);
}

// Transmit and receive data with the RC522
uint8_t RC522_TransceiveData(RC522_HandleTypeDef *rc522, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen) {
    uint8_t waitIRq = 0x30;  // RxIRq and IdleIRq
    uint8_t n;
    uint32_t i;

    // Set bit framing and flush FIFO buffer
    RC522_WriteRegister(rc522, CommandReg, PCD_IDLE);
    RC522_WriteRegister(rc522, ComIrqReg, 0x7F);
    RC522_SetRegisterBits(rc522, FIFOLevelReg, 0x80);

    // Write data to FIFO
    for (i = 0; i < sendLen; i++) {
        RC522_WriteRegister(rc522, FIFODataReg, sendData[i]);
    }
    RC522_WriteRegister(rc522, CommandReg, PCD_TRANSCEIVE);
    RC522_SetRegisterBits(rc522, BitFramingReg, 0x80);  // Start sending

    // Wait for transmission to complete
    i = 2000;
    do {
        n = RC522_ReadRegister(rc522, ComIrqReg);
        i--;
    } while ((i != 0) && !(n & waitIRq) && !(n & 0x01));

    // Stop sending
    RC522_ClearRegisterBits(rc522, BitFramingReg, 0x80);

    if (i != 0) {
        if (!(RC522_ReadRegister(rc522, ErrorReg) & 0x1B)) {
            *backLen = RC522_ReadRegister(rc522, FIFOLevelReg);
            for (i = 0; i < *backLen; i++) {
                backData[i] = RC522_ReadRegister(rc522, FIFODataReg);
            }
            return MI_OK;
        }
        else {
            return MI_ERR;
        }
    }
    return MI_TIMEOUT;
}

// Request for card presence
uint8_t RC522_Request(RC522_HandleTypeDef *rc522, uint8_t reqMode, uint8_t *TagType) {
    uint8_t status;
    uint8_t backBits;

    RC522_WriteRegister(rc522, BitFramingReg, 0x07);
    TagType[0] = reqMode;
    status = RC522_TransceiveData(rc522, TagType, 1, TagType, &backBits);

    if ((status != MI_OK) || (backBits != 0x10)) {
        status = MI_ERR;
    }
    return status;
}

// Anti-collision detection to prevent multiple cards from responding simultaneously
uint8_t RC522_Anticoll(RC522_HandleTypeDef *rc522, uint8_t *serNum) {
    uint8_t status;
    uint8_t i, serNumCheck = 0;
    uint8_t backLen;

    RC522_WriteRegister(rc522, BitFramingReg, 0x00);
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = RC522_TransceiveData(rc522, serNum, 2, serNum, &backLen);

    if (status == MI_OK) {
        for (i = 0; i < 4; i++) {
            serNumCheck ^= serNum[i];
        }
        if (serNumCheck != serNum[4]) {
            status = MI_ERR;
        }
    }
    return status;
}

// Select a card after collision resolution
uint8_t RC522_SelectTag(RC522_HandleTypeDef *rc522, uint8_t *serNum) {
    uint8_t i;
    uint8_t buffer[9];
    uint8_t backLen;
    buffer[0] = PICC_SELECTTAG;
    buffer[1] = 0x70;

    for (i = 0; i < 5; i++) {
        buffer[i + 2] = serNum[i];
    }

    RC522_CalculateCRC(rc522, buffer, 7, &buffer[7]);
    uint8_t status = RC522_TransceiveData(rc522, buffer, 9, buffer, &backLen);
    return (status == MI_OK && backLen == 0x18) ? buffer[0] : MI_ERR;
}

// Authenticate with a specific card using Key A or Key B
uint8_t RC522_Auth(RC522_HandleTypeDef *rc522, uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum) {
    uint8_t i;
    uint8_t buffer[12];
    buffer[0] = authMode;
    buffer[1] = BlockAddr;

    for (i = 0; i < 6; i++) {
        buffer[i + 2] = Sectorkey[i];
    }
    for (i = 0; i < 4; i++) {
        buffer[i + 8] = serNum[i];
    }
    return RC522_TransceiveData(rc522, buffer, 12, buffer, &i);
}

// Read a data block from a card
uint8_t RC522_Read(RC522_HandleTypeDef *rc522, uint8_t blockAddr, uint8_t *recvData) {
    uint8_t buffer[4];
    uint8_t backLen;
    buffer[0] = PICC_READ;
    buffer[1] = blockAddr;
    RC522_CalculateCRC(rc522, buffer, 2, &buffer[2]);
    return RC522_TransceiveData(rc522, buffer, 4, recvData, &backLen);
}

// Write a data block to a card
uint8_t RC522_Write(RC522_HandleTypeDef *rc522, uint8_t blockAddr, uint8_t *writeData) {
    uint8_t buffer[4];
    uint8_t backLen;
    buffer[0] = PICC_WRITE;
    buffer[1] = blockAddr;
    RC522_CalculateCRC(rc522, buffer, 2, &buffer[2]);

    if (RC522_TransceiveData(rc522, buffer, 4, buffer, &backLen) != MI_OK) {
        return MI_ERR;
    }
    return RC522_TransceiveData(rc522, writeData, 16, buffer, &backLen);
}

// Calculate CRC for communication
void RC522_CalculateCRC(RC522_HandleTypeDef *rc522, uint8_t *data, uint8_t length, uint8_t *result) {
    RC522_WriteRegister(rc522, CommandReg, PCD_IDLE);      // Stop any active command
    RC522_WriteRegister(rc522, DivIrqReg, 0x04);           // Clear the CRCIRq interrupt request bit
    RC522_SetRegisterBits(rc522, FIFOLevelReg, 0x80);      // FlushBuffer = 1, FIFO initialization
    for (uint8_t i = 0; i < length; i++) {
        RC522_WriteRegister(rc522, FIFODataReg, data[i]);  // Write data to the FIFO
    }
    RC522_WriteRegister(rc522, CommandReg, PCD_CALCCRC);   // Start the calculation

    // Wait for the CRC calculation to complete
    uint8_t n;
    uint16_t i = 5000;
    while (1) {
        n = RC522_ReadRegister(rc522, DivIrqReg);
        if (n & 0x04) {  // CRCIRq bit set - calculation done
            break;
        }
        if (--i == 0) {  // Timeout condition
            return;
        }
    }

    // Transfer the result from the registers to the result buffer
    result[0] = RC522_ReadRegister(rc522, CRCResultRegL);
    result[1] = RC522_ReadRegister(rc522, CRCResultRegH);
}


// Halt communication with the card
void RC522_Halt(RC522_HandleTypeDef *rc522) {
    uint8_t buffer[4];
    buffer[0] = PICC_HALT;
    buffer[1] = 0;
    RC522_CalculateCRC(rc522, buffer, 2, &buffer[2]);
    RC522_TransceiveData(rc522, buffer, 4, NULL, NULL);
}
