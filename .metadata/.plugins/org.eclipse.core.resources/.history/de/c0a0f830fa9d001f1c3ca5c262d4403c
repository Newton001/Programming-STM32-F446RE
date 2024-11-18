/*
 * rc522.c
 *
 *  Created on: Nov 3, 2024
 *      Author: newton
 */

#include "rc522.h"

// Transfers a byte of data via SPI and returns the received byte
uint8_t RC522_SPI_Transfer(uint8_t tx_data)
{
    uint8_t rx_data;
    HAL_SPI_TransmitReceive(&hspi1, &tx_data, &rx_data, 1, 100);
    return rx_data;
}

// Writes a value to a specific RC522 register
void write_RC522(uint8_t ADD, uint8_t val)
{
    // Pull the Chip Select Pin Down to activate the RFID
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    RC522_SPI_Transfer((ADD << 1) & 0x7E);  // Send address byte with write bit
    RC522_SPI_Transfer(val);  // Send the value to write
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);  // Deactivate RFID
}

// Reads a value from a specific RC522 register
uint8_t read_RC522(uint8_t addr)
{
    uint8_t val;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  // Activate RFID
    RC522_SPI_Transfer(((addr << 1) & 0x7E) | 0x80);  // Send address byte with read bit
    val = RC522_SPI_Transfer(MI_OK);  // Read the value from the register
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);  // Deactivate RFID
    return val;
}

// Sets specific bits of a register
void SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t temp = read_RC522(reg);
    write_RC522(reg, temp | mask);  // Set bits defined by mask
}

// Clears specific bits of a register
void ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t temp = read_RC522(reg);
    write_RC522(reg, temp & (~mask));  // Clear bits defined by mask
}

// Turns on the antenna of the RC522 module
void antenna_on(void)
{
    read_RC522(TxControlReg);
    SetBitMask(TxControlReg, DivlEnReg);
}

// Turns off the antenna of the RC522 module
void antenna_off(void)
{
    ClearBitMask(TxControlReg, DivlEnReg);
}

// Resets the RC522 module
void RC522_Reset(void)
{
    write_RC522(CommandReg, PCD_RESETPHASE);
}

// Initializes the RC522 module
void RC522_Init(void)
{
    // Perform hardware reset
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

    RC522_Reset();

    // Configure timer and other settings for RC522
    write_RC522(TModeReg, 0x8D);
    write_RC522(TPrescalerReg, 0x3E);
    write_RC522(TReloadRegL, 30);
    write_RC522(TReloadRegH, MI_OK);
    write_RC522(TxAutoReg, 0x40);
    write_RC522(ModeReg, 0x3D);

    // Turn on the antenna
    antenna_on();
}

// Sends data to RC522 and gets response
uint8_t RC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen)
{
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;

    // Set IRQ based on the command type
    switch (command)
    {
        case PCD_AUTHENT:
            irqEn = 0x12;
            waitIRq = 0x10;
            break;
        case PCD_TRANSCEIVE:
            irqEn = 0x77;
            waitIRq = 0x30;
            break;
        default:
            break;
    }

    // Enable IRQ, clear interrupt request bits, and flush FIFO
    write_RC522(ComlEnReg, irqEn | 0x80);
    ClearBitMask(ComIrqReg, 0x80);
    SetBitMask(FIFOLevelReg, 0x80);
    write_RC522(CommandReg, PCD_IDLE);

    // Write data to FIFO
    for (i = 0; i < sendLen; ++i)
    {
        write_RC522(FIFODataReg, sendData[i]);
    }

    // Start the command
    write_RC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80);  // Start transmitting data
    }

    // Wait for the command to complete (or timeout)
    i = 2000;
    do
    {
        n = read_RC522(ComIrqReg);
        i--;
    } while ((i != 0) && (n & 0x01) && !(n & waitIRq));

    // Stop sending data
    ClearBitMask(BitFramingReg, 0x80);

    if (i != 0)
    {
        if (!(read_RC522(ErrorReg) & 0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }
            if (command == PCD_TRANSCEIVE)
            {
                n = read_RC522(FIFOLevelReg);
                lastBits = read_RC522(ControlReg) & 0x07;
                if (lastBits)
                {
                    *backLen = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *backLen = n * 8;
                }

                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAX_LEN)
                {
                    n = MAX_LEN;
                }

                // Read the received data from FIFO
                for (i = 0; i < n; ++i)
                {
                    backData[i] = read_RC522(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }
    return status;
}

// Requests to read RFID tag type
uint8_t RC522_Request(uint8_t reqMode, uint8_t *TagType)
{
    uint8_t status;
    uint8_t backBits;

    write_RC522(BitFramingReg, 0x07);  // Set bit framing
    TagType[0] = reqMode;
    status = RC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

    if ((status != MI_OK) || (backBits != 0x10))
    {
        status = MI_ERR;
    }
    return status;
}

// Performs anti-collision detection to find the tag's serial number
uint8_t RC522_Anticoll(uint8_t *serNum)
{
    uint8_t status;
    uint8_t i;
    uint8_t serNumCheck = 0;
    uint8_t unLen;

    write_RC522(BitFramingReg, 0x00);  // Set bit framing to default
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = RC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
        for (i = 0; i < 4; ++i)
        {
            serNumCheck ^= serNum[i];  // Calculate checksum
        }
        if (serNumCheck != serNum[i])
        {
            status = MI_ERR;
        }
    }
    return status;
}

// Calculates CRC for the given data
void CalculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
    uint8_t i, n;
    ClearBitMask(DivIrqReg, 0x04);  // Clear CRC interrupt bit
    SetBitMask(FIFOLevelReg, 0x80);  // Flush FIFO

    for (i = 0; i < len; ++i)
    {
        write_RC522(FIFODataReg, *(pIndata + i));  // Write data to FIFO
    }
    write_RC522(CommandReg, PCD_CALCCRC);  // Start CRC calculation

    // Wait for CRC calculation to complete
    i = 0xFF;
    do
    {
        n = read_RC522(DivIrqReg);
        i--;
    } while ((i != 0) && (n & 0x04));

    // Read CRC result
    pOutData[0] = read_RC522(CRCResultRegL);
    pOutData[1] = read_RC522(CRCResultRegM);
}

// Selects a tag and returns the tag size
uint8_t RC522_SelectTag(uint8_t *serNum)
{
    uint8_t i;
    uint8_t status;
    uint8_t size;
    uint8_t recvBits;
    uint8_t buffer[9];

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;

    // Copy serial number to buffer
    for (i = 0; i < 5; ++i)
    {
        buffer[i + 2] = *(serNum + i);
    }

    CalculateCRC(buffer, 7, &buffer[7]);  // Calculate CRC for buffer
    status = RC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

    if ((status == MI_OK) && (recvBits == 0x18))
    {
        size = buffer[0];
    }
    else
    {
        size = 0;
    }
    return size;
}

// Authenticates a block of memory in the RFID card
uint8_t RC522_Auth(uint8_t authMode, uint8_t blockAddr, uint8_t *sectorKey, uint8_t *serNum)
{
    uint8_t status;
    uint8_t recvBits;
    uint8_t i;
    uint8_t buff[12];

    buff[0] = authMode;
    buff[1] = blockAddr;

    // Copy sector key to buffer
    for (i = 0; i < 6; ++i)
    {
        buff[i + 2] = *(sectorKey + i);
    }
    // Copy serial number to buffer
    for (i = 0; i < 4; ++i)
    {
        buff[i + 8] = *(serNum + i);
    }

    // Send authentication command
    status = RC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(read_RC522(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }
    return status;
}

// Reads a block of memory from the RFID card
uint8_t RC522_Read(uint8_t blockAddr, uint8_t *recvData)
{
    uint8_t status;
    uint8_t unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalculateCRC(recvData, 2, &recvData[2]);  // Calculate CRC for read command
    status = RC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }

    return status;
}

// Writes a block of data to the RFID card
uint8_t RC522_Write(uint8_t blockAddr, uint8_t *writeData)
{
    uint8_t status;
    uint8_t recvBits;
    uint8_t i;
    uint8_t buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalculateCRC(buff, 2, &buff[2]);  // Calculate CRC for write command
    status = RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        // Write 16 bytes of data to the FIFO
        for (i = 0; i < 16; i++)
        {
            buff[i] = *(writeData + i);
        }
        CalculateCRC(buff, 16, &buff[16]);  // Calculate CRC for data
        status = RC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

        if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }

    return status;
}

// Puts the RFID card into halt state
void RC522_Halt(void)
{
    uint8_t unLen;
    uint8_t buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalculateCRC(buff, 2, &buff[2]);  // Calculate CRC for halt command

    RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}
