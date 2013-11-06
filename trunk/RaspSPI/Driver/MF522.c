/*
 * MF522.c
 *
 *  Created on: 03/06/2012
 *      Author: ericsanchez
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string.h>
// RFID HEADERS
#include "MF522.h"
#include  "../spi.h"

#define HW_SPI_SELECT()   		CLR_BIT(25)
#define HW_SPI_UNSELECT() 		SET_BIT(25)
//RFID
static int FileDescriptor;

//DElete please
uchar serNum[5];
//uchar  writeData[16]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100};
//uchar  moneyConsume = 18 ;
//uchar  moneyAdd = 10 ;




void ApiInit(int fd)
{
	FileDescriptor=fd;
}

void Write_MFRC522(uchar addr, uchar val)
{
	HW_SPI_SELECT();
	SPI_transfer(FileDescriptor,(addr<<1)&0x7E);
	SPI_transfer(FileDescriptor,val);
	HW_SPI_UNSELECT();

}

uint8_t Read_MFRC522(uchar addr)
{
	uchar val;
	HW_SPI_SELECT();
	SPI_transfer(FileDescriptor,((addr<<1)&0x7E) | 0x80);
	val =SPI_transfer(FileDescriptor,0x00);
	HW_SPI_UNSELECT();
	return val;
}

void SetBitMask(uchar reg, uchar mask)
{
	uchar tmp;
	tmp = Read_MFRC522(reg);
	Write_MFRC522(reg, tmp | mask);  // set bit mask
}

void ClearBitMask(uchar reg, uchar mask)
{
	uchar tmp;
	tmp = Read_MFRC522(reg);
	Write_MFRC522(reg, tmp & (~mask));  // clear bit mask
}
void AntennaOn(void)
{
	uchar temp;

	temp = Read_MFRC522(TxControlReg);
	if (!(temp & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}

void AntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

void MFRC522_Reset(void)
{
	Write_MFRC522(CommandReg, PCD_RESETPHASE);
}
/*
      Function: void MFRC522_Init(void)
   Description: Initialize all Hardware & Software configuration in RFID Reader.

 */
void MFRC522_Init(void)
{
	MFRC522_Reset();
	Write_MFRC522(TModeReg, 0x8D);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	Write_MFRC522(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg
	Write_MFRC522(TReloadRegL, 30);
	Write_MFRC522(TReloadRegH, 0);

	Write_MFRC522(TxAutoReg, 0x40);		//100%ASK
	Write_MFRC522(ModeReg, 0x3D);		//CRC初始值0x6363	???

	AntennaOn();		//打开天线
}


/*
 * Function: MFRC522_Request
 * Description: Find cards, read the card type No.
 * Input parameters: reqMode - find cards way
 * TagType - Back to the card type
 * 0x4400 = Mifare_UltraLight
 * 0x0400 = Mifare_One (S50)
 * 0x0200 = Mifare_One (S70)
 * 0x0800 = Mifare_Pro (X)
 * 0x4403 = Mifare_DESFire
 * Return value: the successful return MI_OK				*/
uchar MFRC522_Request(uchar reqMode, uchar *TagType)
{
	uchar status;
	uint backBits;			//The received data bit

	Write_MFRC522(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10)){
		status = MI_ERR;
	}

	return status;
}


/*
 * Function name: MFRC522_ToCard
 * Description: RC522 and ISO14443 card communication
 * Input Parameters: command - MF522 command word,
 * SendData - RC522 sent to the card data,
 * SendLen - length of data sent
 * BackData - received the card data is returned,
 * BackLen - returns the bit length of the data
 * Return value: the successful return MI_OK
 */
unsigned char MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
	uchar status = MI_ERR;
	uchar irqEn = 0x00;
	uchar waitIRq = 0x00;
	uchar lastBits;
	uchar n;
	uint i;

	switch (command)
	{
	case PCD_AUTHENT:		//Certification card secret
	{
		irqEn = 0x12;
		waitIRq = 0x10;
		break;
	}
	case PCD_TRANSCEIVE:	//Transmit FIFO data
	{
		irqEn = 0x77;
		waitIRq = 0x30;
		break;
	}
	default:
		break;
	}

	Write_MFRC522(CommIEnReg, irqEn|0x80);	//Interrupt request
	ClearBitMask(CommIrqReg, 0x80);			//Clear all interrupt request bit
	SetBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1, FIFO Initialization

	Write_MFRC522(CommandReg, PCD_IDLE);	//NO action	???   !!! ATENTION

	//Data is written to the FIFO
	for (i=0; i<sendLen; i++)
	{
		Write_MFRC522(FIFODataReg, sendData[i]);
	}
	//Execute command
	Write_MFRC522(CommandReg, command);
	if (command == PCD_TRANSCEIVE)
	{
		SetBitMask(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts
	}

	//Waiting to receive data to complete
	i = 2000;	// i clock frequency adjustment, operating maximum wait time of 25ms	???
	do
	{
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = Read_MFRC522(CommIrqReg);
		i--;
	}
	while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	ClearBitMask(BitFramingReg, 0x80);			//StartSend=0

	if (i != 0)
	{
		if(!(Read_MFRC522(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
		{
			status = MI_OK;
			if (n & irqEn & 0x01){
				status = MI_NOTAGERR;			//??
			}

			if (command == PCD_TRANSCEIVE){
				n = Read_MFRC522(FIFOLevelReg);
				lastBits = Read_MFRC522(ControlReg) & 0x07;
				if (lastBits){
					*backLen = (n-1)*8 + lastBits;
				}
				else{
					*backLen = n*8;
				}
				if (n == 0){
					n = 1;
				}
				if (n > MAX_LEN){
					n = MAX_LEN;
				}
				// Read the FIFO data received
				for (i=0; i<n; i++){
					backData[i] = Read_MFRC522(FIFODataReg);
				}
			}
		}
		else{
			status = MI_ERR;
		}
	}
	//SetBitMask(ControlReg,0x80);           //timer stops
	//Write_MFRC522(CommandReg, PCD_IDLE);
	return status;
}




uchar Desfire_get_version(void)
{
	uchar status;
	uchar i;
	uchar serNumCheck=0;
	uint unLen;
	//ClearBitMask(Status2Reg, 0x08);		//TempSensclear
	//ClearBitMask(CollReg,0x80);			//ValuesAfterColl
	//Write_MFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]
	serNum[0] = 0x60;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 1, serNum, &unLen);
	return status;
}



/*
 * Function name: MFRC522_Anticoll
 * Description: Anti-collision detection, read the card serial number of the selected card
 * Input parameters: serNum - returns the 4 byte card serial number, the 5-byte checksum byte
 * Return value: the successful return MI_OK
 */
uchar MFRC522_Anticoll(uchar *serNum)
{
	uchar status;
	uchar i;
	uchar serNumCheck=0;
	uint unLen;

	//ClearBitMask(Status2Reg, 0x08);		//TempSensclear
	//ClearBitMask(CollReg,0x80);			//ValuesAfterColl
	Write_MFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if (status == MI_OK){
		//Check card serial number
		for (i=0; i<4; i++){
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i]){
			status = MI_ERR;
		}
	}
	//SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1
	return status;
}


/*
 * Function name: CalulateCRC
 * Description: MF522 calculated CRC
 * Input parameters: pIndata - CRC data, len - the length of the data, pOutData - calculate CRC result to be reading
 * Return value: no
 */
void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
	uchar i, n;

	ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
	SetBitMask(FIFOLevelReg, 0x80);			//清FIFO指针
	//Write_MFRC522(CommandReg, PCD_IDLE);

	// FIFO Write Data
	for (i=0; i<len; i++)
	{
		Write_MFRC522(FIFODataReg, *(pIndata+i));
	}
	Write_MFRC522(CommandReg, PCD_CALCCRC);

	// Wait for completion of the CRC calculation
	i = 0xFF;
	do
	{
		n = Read_MFRC522(DivIrqReg);
		i--;
	}
	while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Read the CRC calculation result
	pOutData[0] = Read_MFRC522(CRCResultRegL);
	pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/*
 * Function: MFRC522_SelectTag
 * Description: Select card, read the card memory capacity
 * Input parameters: serNum - incoming card serial number
 * Return value: the successful return of the card capacity
 */
uchar MFRC522_SelectTag(uchar *serNum)
{
	uchar i;
	uchar status;
	uchar size;
	uint recvBits;
	uchar buffer[9];
	//ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0
	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;

	for (i=0; i<5; i++){
		buffer[i+2] = *(serNum+i);
	}

	CalulateCRC(buffer, 7, &buffer[7]);		//??
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
	if ((status == MI_OK) && (recvBits == 0x18)){
		size = buffer[0];
	}
	else{
		size = 0;
	}

	return size;
}


/*
 * Function name: MFRC522_Auth
 * Description: Verify card password
 * Input: authMode - password authentication mode
                  0x60 = Verify A key
                  0x61 = B key validation
              BlockAddr - block address
              Sectorkey - Sector password
              serNum - card serial number, 4 bytes
 * Return value: the successful return MI_OK
 */
uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum)
{
	uchar status;
	uint recvBits;
	uchar i;
	uchar buff[12];

	// Verify command + block address + sectors password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i=0; i<6; i++)
	{
		buff[i+2] = *(Sectorkey+i);
	}
	for (i=0; i<4; i++)
	{
		buff[i+8] = *(serNum+i);
	}
	status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

	if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
	{
		status = MI_ERR;
	}

	return status;
}


/*
 * Function name: MFRC522_Read
 * Description: Read block data
 * Input parameters: blockAddr - block address; recvData - read out blocks of data
 * Return value: the successful return MI_OK
 */
uchar MFRC522_Read(uchar blockAddr, uchar *recvData)
{
	uchar status;
	uint unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	CalulateCRC(recvData,2, &recvData[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

	if ((status != MI_OK) || (unLen != 0x90))
	{
		status = MI_ERR;
	}

	return status;
}


/*
 * Function name: MFRC522_Write
 * Description: write block data
 * Input parameters: blockAddr - block address; writeData - 16 bytes of data to write to the block
 * Return value: the successful return MI_OK
 */
uchar MFRC522_Write(uchar blockAddr, uchar *writeData)
{
	uchar status;
	uint recvBits;
	uchar i;
	uchar buff[18];

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	CalulateCRC(buff, 2, &buff[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	if (status == MI_OK)
	{
		for (i=0; i<16; i++)		//to the FIFO write 16Byte data
		{
			buff[i] = *(writeData+i);
		}
		CalulateCRC(buff, 16, &buff[16]);
		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		{
			status = MI_ERR;
		}
	}

	return status;
}

void MFRC522_Halt(void)
{
	uchar status;
	uint unLen;
	uchar buff[4];

	buff[0] = PICC_HALT;
	buff[1] = 0;
	CalulateCRC(buff, 2, &buff[2]);

	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}
