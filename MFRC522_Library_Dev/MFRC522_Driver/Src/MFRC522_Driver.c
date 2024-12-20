/***************************************************************************************************
 * @file       MFRC522.h
 * @author     Newton Kelvin
 *
 */
#include "MFRC522_Driver.h"
#include "spi.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include"stdio.h"

Uid uid;

// Constants
#define MFRC522_SPI &hspi1

// Internal Constants and Arrays
static const char* const _TypeNamePICC[] = {
		"Unknown type",
		"PICC compliant with ISO/IEC 14443-4",
		"PICC compliant with ISO/IEC 18092 (NFC)",
		"MIFARE Mini, 320 bytes",
		"MIFARE 1KB",
		"MIFARE 4KB",
		"MIFARE Ultralight or Ultralight C",
		"MIFARE Plus",
		"MIFARE TNP3XXX",
		"SAK indicates UID is not complete"
};

static const char* const _ErrorMessage[] = {
		"Unknown error",
		"Success",
		"Error in communication",
		"Collision detected",
		"Timeout in communication",
		"A buffer is not big enough",
		"Internal error in the code, should not happen",
		"Invalid argument",
		"The CRC_A does not match",
		"A MIFARE PICC responded with NAK"
};

// GPIO Helper Functions
void MFRC522_Select(void) {
	HAL_GPIO_WritePin(RC522_CS_GPIO_Port, RC522_CS_Pin, GPIO_PIN_RESET);
}

void MFRC522_Deselect(void) {
	HAL_GPIO_WritePin(RC522_CS_GPIO_Port, RC522_CS_Pin, GPIO_PIN_SET);
}

void MFRC522_Reset(void) {
	HAL_GPIO_WritePin(RC522_RESET_GPIO_Port, RC522_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RC522_RESET_GPIO_Port, RC522_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
}

// Initialization and Configuration
void PCD_Init(void) {
	MFRC522_Reset();
	PCD_WriteRegister(TModeReg, 0x80);
	PCD_WriteRegister(TPrescalerReg, 0xA9);
	PCD_WriteRegister(TReloadRegH, 0x03);
	PCD_WriteRegister(TReloadRegL, 0xE8);
	PCD_WriteRegister(TxASKReg, 0x40);
	PCD_WriteRegister(ModeReg, 0x3D);
	PCD_SetRegisterBits(TxControlReg, 0x03); // Turn on the antenna
	PCD_WriteRegister(RFCfgReg, 0x60); // Set receiver gain to maximum

}

// Low-Level Register Access
void PCD_WriteRegister(uint8_t reg, uint8_t value) {
	uint8_t txData[2] = { (reg & 0x7E), value }; // MSB 0 for write
	MFRC522_Select();
	HAL_SPI_Transmit(MFRC522_SPI, txData, 2, HAL_MAX_DELAY);
	MFRC522_Deselect();
}

void PCD_WriteRegisterMulti(uint8_t reg, uint8_t count, uint8_t *values) {
	uint8_t txData = (reg & 0x7E); // MSB 0 for write
	MFRC522_Select();
	HAL_SPI_Transmit(MFRC522_SPI, &txData, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(MFRC522_SPI, values, count, HAL_MAX_DELAY);
	MFRC522_Deselect();
}

uint8_t PCD_ReadRegister(uint8_t reg) {
	uint8_t txData = (reg & 0x7E) | 0x80; // MSB 1 for read
	uint8_t rxData = 0;
	MFRC522_Select();
	HAL_SPI_Transmit(MFRC522_SPI, &txData, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(MFRC522_SPI, &rxData, 1, HAL_MAX_DELAY);
	MFRC522_Deselect();
	return rxData;
}

void PCD_ReadRegisterMulti(uint8_t reg, uint8_t count, uint8_t *values) {
	uint8_t txData = (reg & 0x7E) | 0x80; // MSB 1 for read
	MFRC522_Select();
	HAL_SPI_Transmit(MFRC522_SPI, &txData, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(MFRC522_SPI, values, count, HAL_MAX_DELAY);
	MFRC522_Deselect();
}

void PCD_SetRegisterBits(uint8_t reg, uint8_t mask) {
	uint8_t value = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, value | mask);
}

void PCD_ClrRegisterBits(uint8_t reg, uint8_t mask) {
	uint8_t value = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, value & ~mask);
}

// Communication Functions
uint8_t PCD_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen,
		uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign,
		bool checkCRC) {
	uint8_t n, _validBits = 0;
	uint32_t i;

	// Prepare values for BitFramingReg
	uint8_t txLastBits = validBits ? *validBits : 0;
	uint8_t bitFraming = (rxAlign << 4) + txLastBits;   // RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]

	PCD_WriteRegister(CommandReg, PCD_Idle);            // Stop any active command.
	PCD_WriteRegister(ComIrqReg, 0x7F);                 // Clear all seven interrupt request bits
	PCD_SetRegisterBits(FIFOLevelReg, 0x80);            // FlushBuffer = 1, FIFO initialization
	PCD_WriteRegisterMulti(FIFODataReg, sendLen, sendData);  // Write sendData to the FIFO
	PCD_WriteRegister(BitFramingReg, bitFraming);       // Bit adjustments
	PCD_WriteRegister(CommandReg, command);             // Execute the command
	if (command == PCD_Transceive)
	{
		PCD_SetRegisterBits(BitFramingReg, 0x80);      // StartSend=1, transmission of data starts
	}

	// Wait for the command to complete.
	// In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
	// Each iteration of the do-while-loop takes 17.86us.
	i = 2000;
	while (1)
	{
		n = PCD_ReadRegister(ComIrqReg);  // ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq   HiAlertIRq LoAlertIRq ErrIRq TimerIRq
		if (n & waitIRq)
		{          // One of the interrupts that signal success has been set.
			break;
		}

		if (n & 0x01)
		{           // Timer interrupt - nothing received in 25ms
			return STATUS_TIMEOUT;
		}

		if (--i == 0)
		{           // The emergency break. If all other condions fail we will eventually terminate on this one after 35.7ms. Communication with the MFRC522 might be down.
			return STATUS_TIMEOUT;
		}
	}

	// Stop now if any errors except collisions were detected.
	uint8_t errorRegValue = PCD_ReadRegister(ErrorReg); // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl   CollErr CRCErr ParityErr ProtocolErr
	if (errorRegValue & 0x13)
	{  // BufferOvfl ParityErr ProtocolErr
		return STATUS_ERROR;
	}

	// If the caller wants data back, get it from the MFRC522.
	if (backData && backLen)
	{
		n = PCD_ReadRegister(FIFOLevelReg);           // Number of bytes in the FIFO
		if (n > *backLen)
		{
			return STATUS_NO_ROOM;
		}

		*backLen = n;                       // Number of bytes returned
		PCD_ReadRegisterMulti(FIFODataReg, n, backData);    // Get received data from FIFO
		_validBits = PCD_ReadRegister(ControlReg) & 0x07; // RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
		if (validBits)
		{
			*validBits = _validBits;
		}
	}

	// Tell about collisions
	if (errorRegValue & 0x08)
	{ // CollErr
		return STATUS_COLLISION;
	}

	// Perform CRC_A validation if requested.
	if (backData && backLen && checkCRC)
	{
		// In this case a MIFARE Classic NAK is not OK.
		if ((*backLen == 1) && (_validBits == 4))
		{
			return STATUS_MIFARE_NACK;
		}

		// We need at least the CRC_A value and all 8 bits of the last byte must be received.
		if ((*backLen < 2) || (_validBits != 0))
		{
			return STATUS_CRC_WRONG;
		}

		// Verify CRC_A - do our own calculation and store the control in controlBuffer.
		uint8_t controlBuffer[2];
		n = PCD_CalculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
		if (n != STATUS_OK)
		{
			return n;
		}

		if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1]))
		{
			return STATUS_CRC_WRONG;
		}
	}

	return STATUS_OK;
}

uint8_t PCD_TransceiveDataBase(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen,
		TransceiveConfig *config) {
	uint8_t defaultValidBits = 0;
	return PCD_CommunicateWithPICC(PCD_Transceive, 0x30, sendData, sendLen, backData, backLen,
			config ? config->validBits : &defaultValidBits,
					config ? config->rxAlign : 0,
							config ? config->checkCRC : false);
}

// PICC Commands
uint8_t PICC_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize) {
	return PICC_REQA_or_WUPA(PICC_CMD_REQA, bufferATQA, bufferSize);
}

uint8_t PICC_WakeupA(uint8_t *bufferATQA, uint8_t *bufferSize) {
	return PICC_REQA_or_WUPA(PICC_CMD_WUPA, bufferATQA, bufferSize);
}

uint8_t PICC_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize) {
	uint8_t validBits = 7; // 7 bits for short frame
	PCD_ClrRegisterBits(CollReg, 0x80); // Clear collision bits
	return PCD_TransceiveDataBase(&command, 1, bufferATQA, bufferSize,
			&(TransceiveConfig){.validBits = &validBits});
}

PCD_TransceiveData2(uint8_t *sendData,
                                    uint8_t sendLen,
                                    uint8_t *backData,
                                    uint8_t *backLen,
                                    uint8_t *validBits,
                                    uint8_t rxAlign,
                                    bool    checkCRC)
{
  uint8_t waitIRq = 0x30;    // RxIRq and IdleIRq
  return PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC);
}

uint8_t PICC_Select(Uid *uid, uint8_t validBits) {
    bool uidComplete = false;
    bool selectDone = false;
    bool useCascadeTag;
    uint8_t cascadeLevel = 1;
    uint8_t result;
    uint8_t count;
    uint8_t index;
    uint8_t uidIndex;
    uint8_t currentLevelKnownBits;
    uint8_t buffer[9];
    uint8_t bufferUsed;
    uint8_t responseBuffer[10];
    uint8_t responseLength;
    uint8_t txLastBits;
    uint32_t retryCounter;

    // Sanity check
    if (validBits > 80) {
        return STATUS_INVALID;
    }

    // Prepare MFRC522
    PCD_ClrRegisterBits(CollReg, 0x80);
    PCD_SetRegisterBits(TxControlReg, 0x03); // Enable the antenna (Tx1 and Tx2)

    // Adjust RF gain to a higher level to enhance signal
    PCD_WriteRegister(RFCfgReg, 0x70); // Set receiver gain to maximum level

    // Loop until UID is fully known
    while (!uidComplete) {
        // Set the Cascade Level in the SEL byte
        switch (cascadeLevel) {
            case 1:
                buffer[0] = PICC_CMD_SEL_CL1;
                uidIndex = 0;
                useCascadeTag = (validBits && uid->size > 4);
                break;
            case 2:
                buffer[0] = PICC_CMD_SEL_CL2;
                uidIndex = 3;
                useCascadeTag = (validBits && uid->size > 7);
                break;
            case 3:
                buffer[0] = PICC_CMD_SEL_CL3;
                uidIndex = 6;
                useCascadeTag = false;
                break;
            default:
                return STATUS_INTERNAL_ERROR;
        }

        // Calculate the number of known UID bits in this level
        currentLevelKnownBits = (validBits > (8 * uidIndex)) ? validBits - (8 * uidIndex) : 0;

        // Build command buffer
        index = 2;
        if (useCascadeTag) {
            buffer[index++] = PICC_CMD_CT;
        }

        uint8_t bytesToCopy = (currentLevelKnownBits / 8) + ((currentLevelKnownBits % 8) ? 1 : 0);
        bytesToCopy = (bytesToCopy > (useCascadeTag ? 3 : 4)) ? (useCascadeTag ? 3 : 4) : bytesToCopy;

        for (count = 0; count < bytesToCopy; count++) {
            buffer[index++] = uid->uidByte[uidIndex + count];
        }

        if (useCascadeTag) {
            currentLevelKnownBits += 8;
        }

        // Execute anticollision or select command, based on currentLevelKnownBits
        selectDone = false;
        retryCounter = 10; // Set maximum retry attempts
        while (!selectDone) {

            if (currentLevelKnownBits >= 32) {
                // All UID bits are known in this level, proceed with SELECT command
                buffer[1] = 0x70; // NVB: Seven whole bytes
                buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5]; // Calculate BCC

                result = PCD_CalculateCRC(buffer, 7, &buffer[7]);
                if (result != STATUS_OK) {
                    printf("CRC Calculation failed at SELECT step. Result: %d\n", result);
                    return result;
                }

                bufferUsed = 9;
                txLastBits = 0;
                responseLength = sizeof(responseBuffer);
            } else {
                // Perform anticollision
                txLastBits = currentLevelKnownBits % 8;
                count = currentLevelKnownBits / 8;
                index = 2 + count;
                buffer[1] = (index << 4) + txLastBits;
                bufferUsed = index + (txLastBits ? 1 : 0);
                responseLength = sizeof(responseBuffer);
            }

            // Debugging logs
            printf("Buffer Used: %u, txLastBits: %u, responseLength: %u\n", bufferUsed, txLastBits, responseLength);
            printf("Transmitting Buffer: ");


            for (uint8_t i = 0; i < bufferUsed; i++) {
                printf("0x%02X ", buffer[i]);
            }
            printf("\n");

            // Set bit adjustments
            PCD_WriteRegister(BitFramingReg, (txLastBits << 4) + txLastBits);

            uint8_t rxAlign = txLastBits;

            // Transmit and receive data
            result = PCD_TransceiveData2(buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits,rxAlign);

            if (result == STATUS_OK) {
                if (currentLevelKnownBits >= 32) {
                    selectDone = true;
                } else {
                    currentLevelKnownBits = 32;
                }
            } else if (result == STATUS_COLLISION) {
                uint8_t collisionPos = PCD_ReadRegister(CollReg) & 0x1F;
                if (collisionPos == 0) {
                    collisionPos = 32;
                }
                if (collisionPos <= currentLevelKnownBits) {
                    printf("Collision occurred at position: %u\n", collisionPos);
                    return STATUS_COLLISION;
                }

                currentLevelKnownBits = collisionPos;
                count = (collisionPos - 1) % 8;
                index = 1 + (collisionPos / 8) + (count ? 1 : 0);
                buffer[index] |= (1 << count);
            } else if (result == STATUS_TIMEOUT) {
                printf("Timeout occurred. Retrying... Attempts left: %u\n", retryCounter);
            } else {
                printf("Transceive failed with status: %u\n", result);
                return result;
            }
        }

        if (retryCounter == 0) {
            printf("Failed to get a response after maximum retries. Resetting MFRC522 and retrying.\n");
            MFRC522_Reset();
            PCD_Init();
            return STATUS_TIMEOUT;
        }

        // Copy the found UID bytes to uid->uidByte[]
        index = (buffer[2] == PICC_CMD_CT) ? 3 : 2;
        bytesToCopy = (buffer[2] == PICC_CMD_CT) ? 3 : 4;
        for (count = 0; count < bytesToCopy; count++) {
            uid->uidByte[uidIndex + count] = buffer[index++];
        }

        // Check response SAK
        if (responseLength != 3 || txLastBits != 0) {
            printf("SAK response incorrect: responseLength = %u, txLastBits = %u\n", responseLength, txLastBits);
            return STATUS_ERROR;
        }

        result = PCD_CalculateCRC(responseBuffer, 1, &buffer[2]);
        if (result != STATUS_OK) {
            printf("CRC check failed for SAK response. Result: %d\n", result);
            return result;
        }
        if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2])) {
            printf("SAK CRC mismatch.\n");
            return STATUS_CRC_WRONG;
        }

        if (responseBuffer[0] & 0x04) {
            cascadeLevel++;
        } else {
            uidComplete = true;
            uid->sak = responseBuffer[0];
        }
    }

    uid->size = 3 * cascadeLevel + 1;
    printf("UID size: %u bytes\n", uid->size);
    return STATUS_OK;
}



uint8_t PICC_HaltA(void) {
	uint8_t buffer[4];
	buffer[0] = PICC_CMD_HLTA;
	buffer[1] = 0;
	uint8_t result = PCD_CalculateCRC(buffer, 2, &buffer[2]);
	if (result != STATUS_OK) return result;
	result = PCD_TransceiveDataBase(buffer, 4, NULL, NULL,
			&(TransceiveConfig){.checkCRC = false});
	return result == STATUS_TIMEOUT ? STATUS_OK : STATUS_ERROR;
}

// MIFARE Commands
uint8_t PCD_Authenticate(uint8_t command, uint8_t blockAddr, MIFARE_Key *key, Uid *uid) {
	uint8_t buffer[12];
	buffer[0] = command;
	buffer[1] = blockAddr;
	memcpy(&buffer[2], key->keyByte, MF_KEY_SIZE);
	memcpy(&buffer[8], uid->uidByte, 4);
	return PCD_CommunicateWithPICC(PCD_MFAuthent, 0x10, buffer, 12, NULL, NULL, NULL, 0, false);
}

void PCD_StopCrypto1(void) {
	PCD_ClrRegisterBits(Status2Reg, 0x08);
}

uint8_t MIFARE_Read(uint8_t blockAddr, uint8_t *buffer, uint8_t *bufferSize) {
	if (buffer == NULL || *bufferSize < 18) {
		return STATUS_NO_ROOM;
	}

	uint8_t cmdBuffer[4] = {PICC_CMD_MF_READ, blockAddr};

	uint8_t result = PCD_TransceiveData(cmdBuffer, 2, buffer, bufferSize, 0);
	return result;
}


uint8_t MIFARE_Write(uint8_t blockAddr, uint8_t *buffer, uint8_t bufferSize) {
	if (buffer == NULL || bufferSize < 16) {
		return STATUS_INVALID;
	}

	uint8_t cmdBuffer[2] = {PICC_CMD_MF_WRITE, blockAddr};

	uint8_t result = PCD_MIFARE_Transceive(cmdBuffer, 2, false);
	if (result != STATUS_OK) {
		return result;
	}

	result = PCD_MIFARE_Transceive(buffer, bufferSize, false);
	return result;
}





uint8_t MIFARE_UltralightWrite(uint8_t page, uint8_t *buffer, uint8_t bufferSize) {
	if (buffer == NULL || bufferSize < 4) {
		return STATUS_INVALID;
	}

	uint8_t cmdBuffer[6] = {PICC_CMD_UL_WRITE, page};
	memcpy(&cmdBuffer[2], buffer, 4);

	return PCD_MIFARE_Transceive(cmdBuffer, 6, false);
}





uint8_t MIFARE_Decrement(uint8_t blockAddr, uint32_t delta)
{
	return MIFARE_TwoStepHelper(PICC_CMD_MF_DECREMENT, blockAddr, delta);
}

uint8_t MIFARE_Increment(uint8_t blockAddr, uint32_t delta)
{
	return MIFARE_TwoStepHelper(PICC_CMD_MF_INCREMENT, blockAddr, delta);
}

uint8_t MIFARE_Restore(uint8_t blockAddr)
{
	// The datasheet describes Restore as a two step operation, but does not explain what data to transfer in step 2.
	// Doing only a single step does not work, so I chose to transfer 0L in step two.
	return MIFARE_TwoStepHelper(PICC_CMD_MF_RESTORE, blockAddr, 0L);
}

uint8_t MIFARE_Transfer(uint8_t blockAddr) {
	uint8_t cmdBuffer[2] = {PICC_CMD_MF_TRANSFER, blockAddr};
	return PCD_MIFARE_Transceive(cmdBuffer, 2, false);
}


// Utilities
uint8_t PICC_GetType(uint8_t sak)
{
	uint8_t retType = PICC_TYPE_UNKNOWN;

	if (sak & 0x04)
	{ // UID not complete
		retType = PICC_TYPE_NOT_COMPLETE;
	}
	else
	{
		switch (sak)
		{
		case 0x09: retType = PICC_TYPE_MIFARE_MINI; break;
		case 0x08: retType = PICC_TYPE_MIFARE_1K;   break;
		case 0x18: retType = PICC_TYPE_MIFARE_4K;   break;
		case 0x00: retType = PICC_TYPE_MIFARE_UL;   break;
		case 0x10:
		case 0x11: retType = PICC_TYPE_MIFARE_PLUS; break;
		case 0x01: retType = PICC_TYPE_TNP3XXX;     break;
		default:
			if (sak & 0x20)
			{
				retType = PICC_TYPE_ISO_14443_4;
			}
			else if (sak & 0x40)
			{
				retType = PICC_TYPE_ISO_18092;
			}
			break;
		}
	}

	return (retType);
}


char* PICC_GetTypeName(uint8_t type) {
	return (type < MFRC522_MaxPICCs) ? (char *)_TypeNamePICC[type] : "Invalid type";
}

char* GetStatusCodeName(uint8_t code) {
	return (code < MFRC522_MaxError) ? (char *)_ErrorMessage[code] : "Invalid code";
}

void MIFARE_SetAccessBits(uint8_t *accessBitBuffer, uint8_t g0, uint8_t g1, uint8_t g2, uint8_t g3) {
	uint8_t c1 = ((g3 & 4) << 1) | ((g2 & 4) >> 0) | ((g1 & 4) >> 1) | ((g0 & 4) >> 2);
	uint8_t c2 = ((g3 & 2) << 2) | ((g2 & 2) << 1) | ((g1 & 2) >> 0) | ((g0 & 2) >> 1);
	uint8_t c3 = ((g3 & 1) << 3) | ((g2 & 1) << 2) | ((g1 & 1) << 1) | ((g0 & 1) >> 0);
	accessBitBuffer[0] = (~c2 << 4) | (~c1 & 0xF);
	accessBitBuffer[1] = (c1 << 4) | (~c3 & 0xF);
	accessBitBuffer[2] = (c3 << 4) | (c2 & 0xF);
}

bool PICC_IsNewCardPresent(void) {
	uint8_t bufferATQA[4];
	uint8_t bufferSize = sizeof(bufferATQA);
	return PICC_RequestA(bufferATQA, &bufferSize) == STATUS_OK;
}

bool PICC_ReadCardSerial(void) {
	return PICC_Select(&uid, 0) == STATUS_OK;
}


uint8_t MIFARE_TwoStepHelper(uint8_t command, uint8_t blockAddr, uint32_t data) {
    uint8_t cmdBuffer[2] = {command, blockAddr}; // Step 1: Command and block address

    // Step 1: Send command and block address
    uint8_t result = PCD_MIFARE_Transceive(cmdBuffer, 2, false); // `false` indicates timeout is not acceptable
    if (result != STATUS_OK) {
        return result; // Return immediately if Step 1 fails
    }

    // Step 2: Transfer the data
    result = PCD_MIFARE_Transceive((uint8_t *)&data, 4, true); // `true` indicates timeout is acceptable
    return result; // Return the result of Step 2
}


uint8_t PCD_TransceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t validBits) {
	uint8_t waitIRq = 0x30; // RxIRq and IdleIRq
	uint8_t rxAlign = 0;    // No alignment adjustment needed
	return PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, &validBits, rxAlign, false);
}


uint8_t PCD_MIFARE_Transceive(uint8_t *sendData, uint8_t sendLen, bool acceptTimeout) {
	uint8_t waitIRq = 0x30;    // RxIRq and IdleIRq
	uint8_t backData[18];      // Temporary buffer for response
	uint8_t backLen = sizeof(backData);
	uint8_t validBits = 0;

	uint8_t result = PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, sendData, sendLen, backData, &backLen, &validBits, 0, true);

	if (acceptTimeout && result == STATUS_TIMEOUT) {
		return STATUS_OK; // Accept timeout as success
	}

	if (result != STATUS_OK || backLen != 1 || validBits != 4 || backData[0] != MF_ACK) {
		return STATUS_ERROR; // Verify ACK
	}

	return STATUS_OK;
}

uint8_t PCD_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result) {
    PCD_WriteRegister(CommandReg, PCD_Idle);                // Stop any active command.
    PCD_SetRegisterBits(DivIrqReg, 0x04);                   // Clear the CRCIRq interrupt request bit.
    PCD_WriteRegister(FIFOLevelReg, 0x80);                  // FlushBuffer = 1, FIFO initialization.

    // Write data to FIFO
    PCD_WriteRegisterMulti(FIFODataReg, length, data);      // Write data to the FIFO

    // Start the CRC calculation
    PCD_WriteRegister(CommandReg, PCD_CalcCRC);

    // Wait for the CRC calculation to complete
    uint32_t i = 5000;  // Arbitrary timeout value
    uint8_t n;
    do {
        n = PCD_ReadRegister(DivIrqReg);  // Read the DivIrqReg to check for the CRCIRq bit
        if (n & 0x04) {                    // Bit 3 in the register indicates CRC calculation is complete
            break;
        }
    } while (--i);

    // Check if the calculation timed out
    if (i == 0) {
        return STATUS_TIMEOUT;
    }

    // CRC is ready - read it from the registers
    result[0] = PCD_ReadRegister(CRCResultRegL);
    result[1] = PCD_ReadRegister(CRCResultRegH);

    return STATUS_OK;
}

