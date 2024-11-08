/*
 * rc522.h
 *
 *  Created on: Nov 3, 2024
 *      Author: newton
 */

#ifndef RC522_H_
#define RC522_H_
#include "stm32f4xx_hal.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi1;
#define MAX_LEN 16

// Registers the MF522
#define PCD_IDLE              0x00               //NO action; Huy bo lenh hien hanh
#define PCD_AUTHENT           0x0E               //Accomplish the KEY
#define PCD_RECEIVE           0x08               //Nhan du Lieu
#define PCD_TRANSMIT          0x04               //Gui of the place
#define PCD_TRANSCEIVE        0x0C               //Gui and receive data
#define PCD_RESETPHASE        0x0F               //Reset
#define PCD_CALCCRC           0x03               //Calculator CRC

// Registrar setting for Mifare_One
#define PICC_REQIDL           0x26               //Antennas are active, do not cover due to power
#define PICC_REQALL           0x52               //All types
#define PICC_ANTICOLL         0x93               //Chong va cham the
#define PICC_SElECTTAG        0x93               //But choose the
#define PICC_AUTHENT1A        0x60               //Accurate verification
#define PICC_AUTHENT1B        0x61               //Verify movie B
#define PICC_READ             0x30               //Reading
#define PICC_WRITE            0xA0               //Khoi viet
#define PICC_DECREMENT        0xC0               //Remember no
#define PICC_INCREMENT        0xC1               //Nap tien
#define PICC_RESTORE          0xC2               //Switch back to the night
#define PICC_TRANSFER         0xB0               //Included in the night of data
#define PICC_HALT             0x50               //Vao che do ngu

//------------------Registers MFRC522---------------
//Page 0:Command and Status

#define Reserved00					0x00
#define CommandReg 					0x01
#define ComlEnReg 					0x02
#define DivlEnReg 					0x03
#define ComIrqReg 					0x04
#define DivIrqReg 					0x05
#define ErrorReg  					0x06
#define Status1Reg 					0x07
#define Status2Reg 					0x08
#define FIFODataReg 				0x09
#define FIFOLevelReg 				0x0A
#define WaterLevelReg 				0x0B
#define ControlReg 					0x0C
#define BitFramingReg 				0x0D
#define CollReg 					0x0E
#define Reserved01					0x0F

//Page 1:Command
#define     Reserved10               0x10
#define     ModeReg              	 0x11
#define     TxModeReg            	 0x12
#define     RxModeReg            	 0x13
#define     TxControlReg         	 0x14
#define     TxAutoReg            	 0x15
#define     TxSelReg      	         0x16
#define     RxSelReg        	     0x17
#define     RxThresholdReg      	 0x18
#define     DemodReg              	 0x19
#define     Reserved11            	 0x1A
#define     Reserved12            	 0x1B
#define     MifareReg             	 0x1C
#define     Reserved13            	 0x1D
#define     Reserved14            	 0x1E
#define     SerialSpeedReg        	 0x1F

//Page 2:CFG
#define     Reserved20            		0x20
#define     CRCResultRegM         		0x21
#define     CRCResultRegL         		0x22
#define     Reserved21            		0x23
#define     ModWidthReg           		0x24
#define     Reserved22            		0x25
#define     RFCfgReg              		0x26
#define     GsNReg                		0x27
#define     CWGsPReg	          		0x28
#define     ModGsPReg             		0x29
#define     TModeReg              		0x2A
#define     TPrescalerReg         		0x2B
#define     TReloadRegH           		0x2C
#define     TReloadRegL           		0x2D
#define     TCounterValueRegH     		0x2E
#define     TCounterValueRegL     		0x2F

//Page 3:TestRegister
#define     Reserved30            		0x30
#define     TestSel1Reg           		0x31
#define     TestSel2Reg           		0x32
#define     TestPinEnReg         		0x33
#define     TestPinValueReg      		0x34
#define     TestBusReg           		0x35
#define     AutoTestReg          		0x36
#define     VersionReg           		0x37
#define     AnalogTestReg        		0x38
#define     TestDAC1Reg          		0x39
#define     TestDAC2Reg          		0x3A
#define     TestADCReg          		0x3B
#define     Reserved31          		0x3C
#define     Reserved32          		0x3D
#define     Reserved33          		0x3E
#define     Reserved34			  		0x3F

// Communication Check
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2

// Function Prototypes
void RC522_Init(void);
uint8_t RC522_Request(uint8_t reqMode, uint8_t *TagType);
uint8_t RC522_Anticoll(uint8_t *serNum);
uint8_t RC522_SelectTag(uint8_t *serNum);
uint8_t RC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum);
uint8_t RC522_Write(uint8_t blockAddr, uint8_t *writeData);
uint8_t RC522_Read(uint8_t blockAddr, uint8_t *recvData);
void RC522_Halt(void);
void RC522_Reset(void);


#endif /* RC522_H_ */