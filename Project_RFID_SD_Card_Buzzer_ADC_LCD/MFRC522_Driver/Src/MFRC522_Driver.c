/***************************************************************************************************
 * @file       MFRC522.h
 * @author     Newton Kelvin
 *
 */

#include "spi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "MFRC522_Driver.h"


// Function to control the Chip Select (CS) line
static void MFRC522_Select(void) {
	HAL_GPIO_WritePin(GPIOB, MFRC522_CS_N_Pin, GPIO_PIN_RESET);
}

static void MFRC522_Deselect(void) {
	HAL_GPIO_WritePin(GPIOB, MFRC522_CS_N_Pin, GPIO_PIN_SET);
}

/**
 * @brief Writes a single byte to a specified register in the MFRC522 chip.
 * @param reg: The address of the register to write to (with MSB set to 0 for write).
 * @param value: The value to write to the register.
 */
void PCD_WriteRegister(uint8_t reg, uint8_t value) {
	uint8_t data[2];
	data[0] = (reg & 0x7E); // Address format: MSB must be 0 for write
	data[1] = value;

	MFRC522_Select();
	HAL_SPI_Transmit(&hspi2, data, 2, HAL_MAX_DELAY);
	MFRC522_Deselect();
}

/**
 * @brief Writes multiple bytes to a specified register in the MFRC522 chip.
 * @param reg: The address of the register to write to (already shifted).
 * @param count: The number of bytes to write.
 * @param values: Pointer to the array of bytes to write.
 */

void PCD_WriteRegisterMulti(uint8_t reg, uint8_t count, const uint8_t *values) {
	// Allocate a buffer for the data (register address + data bytes)
	uint8_t buffer[count + 1];
	buffer[0] = reg; // First byte is the register address

	// Copy the values to the buffer
	for (uint8_t i = 0; i < count; i++) {
		buffer[i + 1] = values[i];
	}

	// Start the SPI communication
	MFRC522_Select();

	// Send the buffer over SPI
	HAL_SPI_Transmit(&hspi2, buffer, count + 1, HAL_MAX_DELAY);

	// End the SPI communication
	MFRC522_Deselect();
}

uint8_t PCD_ReadRegister(uint8_t reg) {
	uint8_t txData = (reg & 0x7E) | 0x80; // Address format: MSB set for read
	uint8_t rxData = 0;

	MFRC522_Select();

	// Transmit the register address and read the response
	HAL_SPI_Transmit(&hspi2, &txData, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi2, &rxData, 1, HAL_MAX_DELAY);

	MFRC522_Deselect();

	return rxData;
}

/**
 * @brief Resets the MFRC522 chip by toggling the reset pin.
 */
void PCD_Reset(void) {
	HAL_GPIO_WritePin(MFRC522_RESET_GPIO_Port, MFRC522_RESET_Pin, GPIO_PIN_RESET); // Pull reset LOW
	HAL_Delay(50);                                                            // Hold for 50 ms
	HAL_GPIO_WritePin(MFRC522_RESET_GPIO_Port, MFRC522_RESET_Pin, GPIO_PIN_SET);  // Pull reset HIGH
	HAL_Delay(50);                                                            // Wait for the chip to initialize
}

/**
 * @brief Sets specific bits in a register without altering other bits.
 * @param reg: The register address to modify.
 * @param mask: The bitmask indicating which bits to set (1 = set bit, 0 = leave bit unchanged).
 */

void PCD_SetRegisterBits(uint8_t reg, uint8_t mask) {
	// Read the current value of the register
	uint8_t current = PCD_ReadRegister(reg);

	// Set the bits indicated by the mask
	PCD_WriteRegister(reg, current | mask);
}

/**
 * @brief Turns the MFRC522 antenna on by enabling the TX1 and TX2 control bits in the TxControlReg register.
 */
void PCD_AntennaOn(void) {
	uint8_t value = PCD_ReadRegister(TxControlReg);
	if ((value & 0x03) != 0x03)
	{
		PCD_WriteRegister(TxControlReg, value | 0x03);
	}
	PCD_WriteRegister(RFCfgReg, (PCD_ReadRegister(RFCfgReg) & 0x8F) | 0x70); // Set TxGain to maximum

}


void PCD_Init(void) {

	PCD_Reset();
	// Step 2: Configure the Timer
	PCD_WriteRegister(TModeReg, 0x80);       // Timer mode (auto-restart enabled)
	PCD_WriteRegister(TPrescalerReg, 0xA9); // Timer prescaler (recommended value)
	PCD_WriteRegister(TReloadRegH, 0x03);     // Timer reload high byte
	PCD_WriteRegister(TReloadRegL, 0xE8);      // Timer reload low byte
	PCD_WriteRegister(TxASKReg, 0x40);
	PCD_WriteRegister(ModeReg, 0x3D);
	PCD_WriteRegister(RFCfgReg, (0x07<<4)); // Set Rx Gain to max
	// Step 3: Configure the antenna gain
	PCD_WriteRegister(RFCfgReg, 0x70); // Set maximum receiver gain

	// Step 4: Turn on the antenna
	PCD_AntennaOn();

	// Step 5: Verify initialization by reading the version
	uint8_t version = PCD_ReadRegister(VersionReg);
	if (version != 0x91 && version != 0x92) {
		// Initialization failed
		printf("Error: MFRC522 initialization failed. VersionReg=0x%02X\n", version);
	}
	else {
		printf("MFRC522 initialized successfully. VersionReg=0x%02X\n", version);
	}

}

/**
 * @brief Reads multiple bytes from a specified register in the MFRC522 chip.
 * @param reg: The address of the register to read from (already shifted).
 * @param count: The number of bytes to read.
 * @param values: Pointer to the buffer where the read data will be stored.
 */
void PCD_ReadRegisterMulti(uint8_t reg, uint8_t count, uint8_t *values) {
	if (count == 0) return; // No bytes to read

	for (uint8_t i = 0; i < count; i++) {
		values[i] = PCD_ReadRegister(reg); // Read each byte using PCD_ReadRegister
	}
}


void PCD_ClearFIFO(void) {
	PCD_WriteRegister(CommandReg, 0x10); // Flush the FIFO buffer
}

void PCD_ClrRegisterBits(uint8_t reg, uint8_t mask) {
	// Read the current value of the register
	uint8_t tmp = PCD_ReadRegister(reg);

	// Write the modified value back to the register
	PCD_WriteRegister(reg, (tmp & (~mask)));
}

/**
 * @brief Calculates the CRC using the MFRC522 hardware.
 * @param data Pointer to the data to calculate the CRC for.
 * @param length Length of the data.
 * @param result Pointer to a 2-byte array to store the CRC result (low byte first).
 * @return STATUS_OK on success, or an error code if CRC calculation fails.
 */
uint8_t PCD_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result) {
	// Clear the CRC result registers
	PCD_WriteRegister(DivIrqReg, 0x04); // Clear the CRCIRq bit
	PCD_WriteRegister(CommandReg, PCD_Idle); // Stop any active command

	// Write data to the FIFO buffer
	PCD_WriteRegisterMulti(FIFODataReg, length, data);

	// Start the CRC calculation
	PCD_WriteRegister(CommandReg, PCD_CalcCRC);

	// Wait for the CRC calculation to complete
	uint8_t n;
	for (uint8_t i = 0; i < 255; i++) {
		n = PCD_ReadRegister(DivIrqReg); // Check the DivIrqReg register
		if (n & 0x04) { // CRCIRq bit is set
			break;
		}
	}

	// Check for timeout
	if (!(n & 0x04)) {
		return STATUS_TIMEOUT;
	}

	// Read the CRC result
	result[0] = PCD_ReadRegister(CRCResultRegL); // Low byte
	result[1] = PCD_ReadRegister(CRCResultRegH); // High byte

	return STATUS_OK;
}

/**
 * @brief Communicates with a PICC using the MFRC522.
 *
 * @param command Command to execute (e.g., PCD_Transceive).
 * @param waitIRq IRQ bits to wait for (e.g., RxIRq or TxIRq).
 * @param sendData Pointer to the data to send.
 * @param sendLen Length of the data to send.
 * @param backData Pointer to the buffer to store the received data (optional).
 * @param backLen Pointer to a variable indicating the size of the buffer and where the received length will be stored.
 * @param validBits Pointer to a variable storing the number of valid bits in the last byte (optional).
 * @param rxAlign Bit alignment for the received data.
 * @param checkCRC If true, validates CRC on the received data.
 * @return Status code (e.g., STATUS_OK, STATUS_TIMEOUT, STATUS_ERROR).
 */
uint8_t PCD_CommunicateWithPICC(uint8_t command,
		uint8_t waitIRq,
		uint8_t *sendData,
		uint8_t sendLen,
		uint8_t *backData,
		uint8_t *backLen,
		uint8_t *validBits,
		uint8_t rxAlign,
		bool checkCRC) {
	uint8_t n, _validBits = 0;
	uint32_t i;

	// Prepare values for BitFramingReg
	uint8_t txLastBits = validBits ? *validBits : 0;
	uint8_t bitFraming = (rxAlign << 4) + txLastBits; // RxAlign = BitFramingReg[6..4], TxLastBits = BitFramingReg[2..0]

	// Stop any active command
	PCD_WriteRegister(CommandReg, PCD_Idle);

	// Clear all IRQ bits
	PCD_WriteRegister(ComIrqReg, 0x7F);

	// Flush FIFO
	PCD_SetRegisterBits(FIFOLevelReg, 0x80);

	// Write data to the FIFO
	PCD_WriteRegisterMulti(FIFODataReg, sendLen, sendData);

	// Set BitFramingReg
	PCD_WriteRegister(BitFramingReg, bitFraming);

	// Execute the command
	PCD_WriteRegister(CommandReg, command);

	// Start transmission if Transceive command
	if (command == PCD_Transceive) {
		PCD_SetRegisterBits(BitFramingReg, 0x80); // StartSend=1, transmission starts
	}

	// Wait for the command to complete
	i = 5000; // Timeout counter
	while (1) {
		n = PCD_ReadRegister(ComIrqReg); // Check IRQ bits
		if (n & waitIRq) { // Success signal
			break;
		}
		if (n & 0x01) { // Timer interrupt (timeout)
			return STATUS_TIMEOUT;
		}
		if (--i == 0) { // Emergency timeout
			return STATUS_TIMEOUT;
		}
	}

	// Check for errors
	uint8_t errorRegValue = PCD_ReadRegister(ErrorReg);
	if (errorRegValue & 0x13) { // BufferOvfl, ParityErr, ProtocolErr
		return STATUS_ERROR;
	}

	// Retrieve data if requested
	if (backData && backLen) {
		n = PCD_ReadRegister(FIFOLevelReg); // Number of bytes in FIFO
		if (n > *backLen) {
			return STATUS_NO_ROOM; // Buffer overflow
		}

		*backLen = n; // Update backLen with the number of bytes received

		// Read data from FIFO
		PCD_ReadRegisterMulti(FIFODataReg, n, backData);

		_validBits = PCD_ReadRegister(ControlReg) & 0x07; // Get valid bits in the last byte
		if (validBits) {
			*validBits = _validBits;
		}
	}

	// Handle collisions
	if (errorRegValue & 0x08) { // CollErr
		return STATUS_COLLISION;
	}

	// Perform CRC validation if requested
	if (backData && backLen && checkCRC) {
		if ((*backLen == 1) && (_validBits == 4)) {
			return STATUS_MIFARE_NACK; // MIFARE NAK
		}

		if ((*backLen < 2) || (_validBits != 0)) {
			return STATUS_CRC_WRONG; // Invalid CRC
		}

		uint8_t controlBuffer[2];
		n = PCD_CalculateCRC(backData, *backLen - 2, controlBuffer);
		if (n != STATUS_OK) {
			return n;
		}

		if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1])) {
			return STATUS_CRC_WRONG;
		}
	}

	return STATUS_OK; // Success
}

/**
 * @brief Sends data to the PICC and receives its response.
 *
 * This function wraps PCD_CommunicateWithPICC to simplify sending data to the PICC and receiving a response.
 *
 * @param sendData Pointer to the data to send.
 * @param sendLen Number of bytes to send.
 * @param backData Pointer to a buffer to store the received data (optional).
 * @param backLen Pointer to a variable indicating the size of the buffer and where the received length will be stored.
 * @param validBits Pointer to a variable storing the number of valid bits in the last byte (optional).
 * @param rxAlign Bit alignment for the received data.
 * @param checkCRC If true, validates CRC on the received data.
 * @return Status code (e.g., STATUS_OK, STATUS_TIMEOUT, STATUS_ERROR).
 */
uint8_t PCD_TransceiveData(uint8_t *sendData,
		uint8_t sendLen,
		uint8_t *backData,
		uint8_t *backLen,
		uint8_t *validBits,
		uint8_t rxAlign,
		bool checkCRC) {
	uint8_t waitIRq = 0x30; // Wait for RxIRq and IdleIRq

	// Call the underlying communication function
	return PCD_CommunicateWithPICC(PCD_Transceive,
			waitIRq,
			sendData,
			sendLen,
			backData,
			backLen,
			validBits,
			rxAlign,
			checkCRC);
}

/**
 * @brief Sends a REQA or WUPA command to the PICC.
 *
 * This function is used to detect if a card is in the RF field and ready for communication.
 *
 * @param command The command to send (PICC_CMD_REQA or PICC_CMD_WUPA).
 * @param bufferATQA Pointer to a buffer to store the ATQA response (2 bytes).
 * @param bufferSize Pointer to a variable indicating the buffer size. Must be at least 2.
 * @return Status code (e.g., STATUS_OK, STATUS_TIMEOUT, STATUS_ERROR).
 */
uint8_t PICC_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize) {
	uint8_t validBits;
	uint8_t status;

	if (bufferATQA == NULL || *bufferSize < 2) {
		// The ATQA response is 2 bytes long.
		return STATUS_NO_ROOM;
	}

	// ValuesAfterColl=1 => Bits received after collision are cleared.
	PCD_ClrRegisterBits(CollReg, 0x80);

	// For REQA and WUPA we need the short frame format
	// - transmit only 7 bits of the last (and only) byte. TxLastBits = BitFramingReg[2..0]
	validBits = 7;

	status = PCD_TransceiveData(&command, 1, bufferATQA, bufferSize, &validBits, 0, false);
	if (status != STATUS_OK) {
		return status;
	}

	if ((*bufferSize != 2) || (validBits != 0)) {
		// ATQA must be exactly 16 bits.
		return STATUS_ERROR;
	}

	return STATUS_OK;
}

/*
 * Transmits a REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
 * Beware: When two PICCs are in the field at the same time I often get STATUS_TIMEOUT - probably due do bad antenna design.
 */
uint8_t PICC_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize)
{
	return PICC_REQA_or_WUPA(PICC_CMD_REQA, bufferATQA, bufferSize);
}

/**
 * Transmits a Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
 * Beware: When two PICCs are in the field at the same time I often get STATUS_TIMEOUT - probably due do bad antenna design.
 */
uint8_t PICC_WakeupA(uint8_t *bufferATQA, uint8_t *bufferSize)
{
	return PICC_REQA_or_WUPA(PICC_CMD_WUPA, bufferATQA, bufferSize);
}

uint8_t PICC_Select(Uid *uid, uint8_t validBits) {
	bool uidComplete = false;
	bool selectDone = false;
	bool useCascadeTag;
	uint8_t cascadeLevel = 1;
	uint8_t result;
	uint8_t count;
	uint8_t index;
	uint8_t uidIndex;        // The first index in uid->uidByte[] that is used in the current Cascade Level.
	uint8_t currentLevelKnownBits; // The number of known UID bits in the current Cascade Level.
	uint8_t buffer[9];       // Buffer for SELECT/ANTICOLLISION commands
	uint8_t bufferUsed;      // Bytes used in the buffer
	uint8_t rxAlign;         // Bit alignment for received data
	uint8_t txLastBits;      // Last transmitted bits
	uint8_t *responseBuffer;
	uint8_t responseLength;

	// Check for validBits exceeding limits
	if (validBits > 80) {
		return STATUS_INVALID;
	}

	// Clear collision register
	PCD_ClrRegisterBits(CollReg, 0x80);

	// Cascade loop
	while (!uidComplete) {
		switch (cascadeLevel) {
		case 1:
			buffer[0] = PICC_CMD_SEL_CL1;
			uidIndex = 0;
			useCascadeTag = validBits && (uid->size > 4);
			break;
		case 2:
			buffer[0] = PICC_CMD_SEL_CL2;
			uidIndex = 3;
			useCascadeTag = validBits && (uid->size > 7);
			break;
		case 3:
			buffer[0] = PICC_CMD_SEL_CL3;
			uidIndex = 6;
			useCascadeTag = false;
			break;
		default:
			return STATUS_INTERNAL_ERROR;
		}

		// Calculate the known UID bits for this cascade level
		currentLevelKnownBits = (validBits > (8 * uidIndex)) ? (validBits - (8 * uidIndex)) : 0;

		// Copy UID bytes to buffer
		index = 2;
		if (useCascadeTag) {
			buffer[index++] = PICC_CMD_CT;
		}

		uint8_t bytesToCopy = currentLevelKnownBits / 8 + ((currentLevelKnownBits % 8) ? 1 : 0);
		uint8_t maxBytes = useCascadeTag ? 3 : 4;

		if (bytesToCopy > maxBytes) {
			bytesToCopy = maxBytes;
		}

		for (count = 0; count < bytesToCopy; count++) {
			buffer[index++] = uid->uidByte[uidIndex + count];
		}

		if (useCascadeTag) {
			currentLevelKnownBits += 8;
		}

		// Anti-collision/selection loop
		selectDone = false;
		while (!selectDone) {
			if (currentLevelKnownBits >= 32) {
				buffer[1] = 0x70;
				buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];

				// Calculate CRC_A
				result = PCD_CalculateCRC(buffer, 7, &buffer[7]);
				if (result != STATUS_OK) {
					return result;
				}

				txLastBits = 0;
				bufferUsed = 9;
				responseBuffer = &buffer[6];
				responseLength = 3;
			} else {
				txLastBits = currentLevelKnownBits % 8;
				count = currentLevelKnownBits / 8;
				index = 2 + count;
				buffer[1] = (index << 4) + txLastBits;
				bufferUsed = index + (txLastBits ? 1 : 0);

				responseBuffer = &buffer[index];
				responseLength = sizeof(buffer) - index;
			}

			// Set bit framing
			rxAlign = txLastBits;
			PCD_WriteRegister(BitFramingReg, (rxAlign << 4) + txLastBits);

			// Transmit and receive
			result = PCD_TransceiveData(buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign, false);
			if (result == STATUS_COLLISION) {
				result = PCD_ReadRegister(CollReg);
				if (result & 0x20) {
					return STATUS_COLLISION;
				}

				uint8_t collisionPos = result & 0x1F;
				if (collisionPos == 0) {
					collisionPos = 32;
				}

				if (collisionPos <= currentLevelKnownBits) {
					return STATUS_INTERNAL_ERROR;
				}

				currentLevelKnownBits = collisionPos;
				count = (currentLevelKnownBits - 1) % 8;
				index = 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0);
				buffer[index] |= (1 << count);
			} else if (result != STATUS_OK) {
				return result;
			} else {
				if (currentLevelKnownBits >= 32) {
					selectDone = true;
				} else {
					currentLevelKnownBits = 32;
				}
			}
		}

		index = (buffer[2] == PICC_CMD_CT) ? 3 : 2;
		bytesToCopy = (buffer[2] == PICC_CMD_CT) ? 3 : 4;
		for (count = 0; count < bytesToCopy; count++) {
			uid->uidByte[uidIndex + count] = buffer[index++];
		}

		if (responseLength != 3 || txLastBits != 0) {
			return STATUS_ERROR;
		}

		result = PCD_CalculateCRC(responseBuffer, 1, &buffer[2]);
		if (result != STATUS_OK) {
			return result;
		}

		if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2])) {
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

	return STATUS_OK;
}

/*
 * Instructs a PICC in state ACTIVE(*) to go to state HALT.
 */
uint8_t PICC_HaltA() {
	uint8_t result;
	uint8_t buffer[4];
	uint8_t backLen = 0;     // No response data expected
	uint8_t validBits = 0;   // No specific valid bits expected
	uint8_t rxAlign = 0;     // Default alignment
	bool checkCRC = false;   // No CRC check for HLTA response

	// Build command buffer
	buffer[0] = PICC_CMD_HLTA;
	buffer[1] = 0;

	// Calculate CRC_A
	result = PCD_CalculateCRC(buffer, 2, &buffer[2]);
	if (result == STATUS_OK) {
		// Send the command.
		// The standard says:
		// If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
		// HLTA command, this response shall be interpreted as 'not acknowledge'.
		// We interpret that this way: Only STATUS_TIMEOUT is a success.
		result = PCD_TransceiveData(buffer, sizeof(buffer), NULL, &backLen, &validBits, rxAlign, checkCRC);
		if (result == STATUS_TIMEOUT) {
			result = STATUS_OK; // Successful HLTA
		} else if (result == STATUS_OK) {
			result = STATUS_ERROR; // Any actual response is an error
		}
	}

	return result;
}

/**
 * @brief Authenticate with a MIFARE card.
 *
 * This function performs authentication with a MIFARE card to enable secure communication.
 *
 * @param command The authentication command (PICC_CMD_MF_AUTH_KEY_A or PICC_CMD_MF_AUTH_KEY_B).
 * @param blockAddr The block address to authenticate.
 * @param key Pointer to the MIFARE_Key structure containing the key.
 * @param uid Pointer to the Uid structure containing the card's UID.
 * @return Status code (e.g., STATUS_OK, STATUS_TIMEOUT, STATUS_ERROR).
 */
uint8_t PCD_Authenticate(uint8_t command, uint8_t blockAddr, MIFARE_Key *key, Uid *uid) {
	uint8_t sendData[12];
	uint8_t waitIRq = 0x10; // IdleIRq

	// Build the command buffer
	sendData[0] = command; // Authentication command
	sendData[1] = blockAddr; // Block address

	// Copy the key into the buffer
	for (uint8_t i = 0; i < MF_KEY_SIZE; i++) {
		sendData[2 + i] = key->keyByte[i];
	}

	// Copy the first 4 bytes of the UID into the buffer
	for (uint8_t i = 0; i < 4; i++) {
		sendData[8 + i] = uid->uidByte[i];
	}

	// Start the authentication process
	return PCD_CommunicateWithPICC(PCD_MFAuthent, waitIRq, sendData, sizeof(sendData), NULL, NULL, NULL, 0, false);
}

void PCD_StopCrypto1()
{
	// Clear MFCrypto1On bit
	PCD_ClrRegisterBits(Status2Reg, 0x08); // Status2Reg[7..0] bits are: TempSensClear I2CForceHS reserved reserved   MFCrypto1On ModemState[2:0]
}


uint8_t MIFARE_Read(uint8_t blockAddr, uint8_t *buffer, uint8_t *bufferSize) {
    uint8_t result = STATUS_NO_ROOM;

    // Sanity check
    if ((buffer == NULL) || (*bufferSize < 18)) {
        return result;
    }

    // Build command buffer
    buffer[0] = PICC_CMD_MF_READ; // Read command
    buffer[1] = blockAddr;        // Block address to read

    // Calculate CRC_A
    result = PCD_CalculateCRC(buffer, 2, &buffer[2]);
    if (result != STATUS_OK) {
        return result; // Return if CRC calculation fails
    }

    // Transmit the buffer and receive the response, validate CRC_A
    return PCD_TransceiveData(buffer, 4, buffer, bufferSize, NULL, 0, true);
}

uint8_t PCD_MIFARE_Transceive(uint8_t *sendData, uint8_t sendLen, bool acceptTimeout) {
    uint8_t result;
    uint8_t cmdBuffer[18]; // Room for 16 bytes of data and 2 bytes CRC_A

    // Sanity check
    if (sendData == NULL || sendLen > 16) {
        return STATUS_INVALID;
    }

    // Copy sendData[] to cmdBuffer[] and calculate CRC_A
    memcpy(cmdBuffer, sendData, sendLen);
    result = PCD_CalculateCRC(cmdBuffer, sendLen, &cmdBuffer[sendLen]); // Append CRC_A
    if (result != STATUS_OK) {
        return result; // Return CRC calculation error
    }

    sendLen += 2; // Include CRC_A in the length

    // Variables for the response
    uint8_t waitIRq = 0x30; // RxIRq and IdleIRq
    uint8_t cmdBufferSize = sizeof(cmdBuffer); // Buffer size
    uint8_t validBits = 0; // To capture the valid bits in the response
    uint8_t rxAlign = 0; // Bit position alignment for the receiver
    bool checkCRC = false; // No CRC validation during this step

    // Transceive the data using PCD_CommunicateWithPICC
    result = PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, cmdBuffer, sendLen, cmdBuffer, &cmdBufferSize, &validBits, rxAlign, checkCRC);

    // Handle timeout if it is acceptable
    if (acceptTimeout && result == STATUS_TIMEOUT) {
        return STATUS_OK; // Timeout is acceptable
    }

    if (result != STATUS_OK) {
        return result; // Return communication error
    }

    // Ensure the PICC replied with a 4-bit ACK
    if (cmdBufferSize != 1 || validBits != 4) {
        return STATUS_ERROR; // Invalid response size or bits
    }

    if (cmdBuffer[0] != MF_ACK) {
        return STATUS_MIFARE_NACK; // NACK received from PICC
    }

    return STATUS_OK; // Command executed successfully
}

uint8_t PICC_GetType(uint8_t sak) {
    uint8_t retType = PICC_TYPE_UNKNOWN;

    if (sak & 0x04) {
        // UID not complete
        retType = PICC_TYPE_NOT_COMPLETE;
    } else {
        switch (sak) {
            case 0x09:
                retType = PICC_TYPE_MIFARE_MINI;
                break;
            case 0x08:
                retType = PICC_TYPE_MIFARE_1K;
                break;
            case 0x18:
                retType = PICC_TYPE_MIFARE_4K;
                break;
            case 0x00:
                retType = PICC_TYPE_MIFARE_UL;
                break;
            case 0x10:
            case 0x11:
                retType = PICC_TYPE_MIFARE_PLUS;
                break;
            case 0x01:
                retType = PICC_TYPE_TNP3XXX;
                break;
            default:
                if (sak & 0x20) {
                    retType = PICC_TYPE_ISO_14443_4;
                } else if (sak & 0x40) {
                    retType = PICC_TYPE_ISO_18092;
                }
                break;
        }
    }

    return retType;
}

bool PICC_IsNewCardPresent(void) {
    uint8_t bufferATQA[2];
    uint8_t bufferSize = sizeof(bufferATQA);

    // Reset the MFRC522's internal state and antennas
    PCD_ClrRegisterBits(Status2Reg, 0x08); // Clear MFCrypto1On bit
    PCD_WriteRegister(CommandReg, PCD_Idle);   // Stop any active command
    PCD_ClrRegisterBits(CollReg, 0x80);  // Clear ValuesAfterColl

    // Send a REQA command to check for the presence of a new card
    uint8_t result = PICC_RequestA(bufferATQA, &bufferSize);

    // If no card responds, return false
    if (result != STATUS_OK) {
        return false;
    }

    // A new card is present
    return true;
}

bool PICC_ReadCardSerial(Uid *uid) {
    uint8_t result;

    // Attempt to select the card and retrieve its UID
    result = PICC_Select(uid, 0); // Pass the UID structure and 0 known valid bits
    if (result != STATUS_OK) {
        return false; // Selection failed
    }

    return true; // UID successfully read
}

/**
 * @brief Writes a block of data to a MIFARE card.
 *
 * This function writes up to 16 bytes of data to a specific block on the card.
 *
 * @param blockAddr The block address to write the data to.
 * @param buffer Pointer to the data to write.
 * @param bufferSize The size of the data to write (must be 16).
 * @return Status code (e.g., STATUS_OK, STATUS_TIMEOUT, STATUS_ERROR).
 */
uint8_t MIFARE_Write(uint8_t blockAddr, uint8_t *buffer, uint8_t bufferSize) {
    uint8_t result;
    uint8_t cmdBuffer[18]; // Buffer for data (16 bytes) + CRC_A (2 bytes)

    // Sanity check
    if (buffer == NULL || bufferSize != 16) {
        return STATUS_INVALID;
    }

    // Build the command buffer
    cmdBuffer[0] = PICC_CMD_MF_WRITE; // Write command
    cmdBuffer[1] = blockAddr;         // Block address

    // Calculate CRC_A for the command
    result = PCD_CalculateCRC(cmdBuffer, 2, &cmdBuffer[2]);
    if (result != STATUS_OK) {
        return result; // Return if CRC calculation fails
    }

    // Transmit the command buffer
    result = PCD_TransceiveData(cmdBuffer, 4, NULL, NULL, NULL, 0, false);
    if (result != STATUS_OK) {
        return result; // Return communication error
    }

    // Copy the data to the buffer
    memcpy(cmdBuffer, buffer, bufferSize);

    // Calculate CRC_A for the data
    result = PCD_CalculateCRC(cmdBuffer, bufferSize, &cmdBuffer[bufferSize]);
    if (result != STATUS_OK) {
        return result; // Return if CRC calculation fails
    }

    // Transmit the data buffer
    result = PCD_TransceiveData(cmdBuffer, bufferSize + 2, NULL, NULL, NULL, 0, false);
    if (result != STATUS_OK) {
        return result; // Return communication error
    }

    return STATUS_OK; // Write successful
}

/**
 * @brief Returns the type name of the PICC based on the type.
 * @param type The type of the PICC.
 * @return Pointer to a string containing the type name.
 */
char *PICC_GetTypeName(uint8_t type) {
    switch (type) {
        case PICC_TYPE_ISO_14443_4:
            return "PICC compliant with ISO/IEC 14443-4";
        case PICC_TYPE_ISO_18092:
            return "PICC compliant with ISO/IEC 18092 (NFC)";
        case PICC_TYPE_MIFARE_MINI:
            return "MIFARE Mini, 320 bytes";
        case PICC_TYPE_MIFARE_1K:
            return "MIFARE 1KB";
        case PICC_TYPE_MIFARE_4K:
            return "MIFARE 4KB";
        case PICC_TYPE_MIFARE_UL:
            return "MIFARE Ultralight or Ultralight C";
        case PICC_TYPE_MIFARE_PLUS:
            return "MIFARE Plus";
        case PICC_TYPE_TNP3XXX:
            return "MIFARE TNP3XXX";
        case PICC_TYPE_NOT_COMPLETE:
            return "SAK indicates UID is not complete";
        default:
            return "Unknown type";
    }
}

/**
 * @brief Returns the name of a status code.
 * @param code The status code.
 * @return Pointer to a string containing the status code name.
 */
char *GetStatusCodeName(uint8_t code) {
    switch (code) {
        case STATUS_OK:
            return "Success";
        case STATUS_ERROR:
            return "Error in communication";
        case STATUS_COLLISION:
            return "Collision detected";
        case STATUS_TIMEOUT:
            return "Timeout in communication";
        case STATUS_NO_ROOM:
            return "A buffer is not big enough";
        case STATUS_INTERNAL_ERROR:
            return "Internal error in the code, should not happen";
        case STATUS_INVALID:
            return "Invalid argument";
        case STATUS_CRC_WRONG:
            return "The CRC_A does not match";
        case STATUS_MIFARE_NACK:
            return "A MIFARE PICC responded with NAK";
        default:
            return "Unknown error";
    }
}

/**
 * @brief Sets access bits for MIFARE Classic cards.
 * @param accessBitBuffer Pointer to the buffer where access bits will be written.
 * @param g0 Access bits for Group 0.
 * @param g1 Access bits for Group 1.
 * @param g2 Access bits for Group 2.
 * @param g3 Access bits for Group 3.
 */
void MIFARE_SetAccessBits(uint8_t *accessBitBuffer, uint8_t g0, uint8_t g1, uint8_t g2, uint8_t g3) {
    uint8_t c1 = ((g3 & 4) << 1) | ((g2 & 4) << 0) | ((g1 & 4) >> 1) | ((g0 & 4) >> 2);
    uint8_t c2 = ((g3 & 2) << 2) | ((g2 & 2) << 1) | ((g1 & 2) << 0) | ((g0 & 2) >> 1);
    uint8_t c3 = ((g3 & 1) << 3) | ((g2 & 1) << 2) | ((g1 & 1) << 1) | ((g0 & 1) << 0);

    accessBitBuffer[0] = (~c2 & 0xF) << 4 | (~c1 & 0xF);
    accessBitBuffer[1] = c1 << 4 | (~c3 & 0xF);
    accessBitBuffer[2] = c3 << 4 | c2;
}

/**
 * @brief Performs a two-step operation for MIFARE cards (Decrement, Increment, Restore).
 * @param command The command to execute (e.g., PICC_CMD_MF_DECREMENT).
 * @param blockAddr The block address to operate on.
 * @param data The value to decrement/increment/restore.
 * @return Status code (e.g., STATUS_OK, STATUS_ERROR).
 */
uint8_t MIFARE_TwoStepHelper(uint8_t command, uint8_t blockAddr, uint32_t data) {
    uint8_t result;
    uint8_t cmdBuffer[2];

    // Step 1: Send the command and block address
    cmdBuffer[0] = command;
    cmdBuffer[1] = blockAddr;
    result = PCD_MIFARE_Transceive(cmdBuffer, 2, false);
    if (result != STATUS_OK) {
        return result;
    }

    // Step 2: Send the data (4 bytes)
    uint8_t dataBuffer[4];
    dataBuffer[0] = (uint8_t)(data & 0xFF);
    dataBuffer[1] = (uint8_t)((data >> 8) & 0xFF);
    dataBuffer[2] = (uint8_t)((data >> 16) & 0xFF);
    dataBuffer[3] = (uint8_t)((data >> 24) & 0xFF);
    result = PCD_MIFARE_Transceive(dataBuffer, 4, false);

    return result;
}
