
/*
 * main.c
 * Created: 27/04/2013
 * Author: esanchez.briones@gmail.com
 * Hackaton!, http://www.hackathonmty.org/
 * Description: RFID-RC522 Implementation
 */ 

#include <linux/spi/spidev.h>
#include <string.h>
#include <stdio.h>

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "./Driver/MF522.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

unsigned char sectorKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},//when you try it again, please change it into your new password
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
};
unsigned char sectorNewKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0x19,0x84,0x07,0x15,0x76,0x14},
		//you can set another ket , such as  " 0x19, 0x84, 0x07, 0x15, 0x76, 0x14 "
		//{0x19, 0x84, 0x07, 0x15, 0x76, 0x14, 0xff,0x07,0x80,0x69, 0x19,0x84,0x07,0x15,0x76,0x14},
		// but when loop, please set the  sectorKeyA, the same key, so that RFID module can read the card
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0x19,0x33,0x07,0x15,0x34,0x14},
};
unsigned char mifare_tag_type(unsigned char * b);
uint8_t keya[6] = {1, 1, 1, 1, 1, 1 };
uint8_t keyb[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
// uint8_t keya[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
// uint8_t keyb[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };


static void pabort(const char *s)
{
	perror(s);
	abort();
}
void main(void)
{
	unsigned char i=0;
	unsigned char status;
	unsigned char str[MAX_LEN];
	unsigned char RC_size;
	unsigned char blockAddr;
	unsigned char mynum[20];
	unsigned char serNum[7];

	uint8_t RCV;
	int resulta=0;
	int fd;
	int a=0;



	fd=Init_SPI("/dev/spidev0.0");
	setup_io();

	SET_INPUT(25);
	SET_OUTPUT(25);

	ApiInit(fd);
	MFRC522_Init();

	a=Read_MFRC522(VersionReg);
	printf("MFRC Version Read %02X \r\n",a);


	//	for(a=0;a<10;a++)
	//	{
	//		RCV=Read_MFRC522(VersionReg);
	//		//RCV=SPI_transfer(fd,a);
	//		printf("RCV %02X \r\n",RCV);
	//		SET_BIT(25);
	//        sleep(1);
	//        CLR_BIT(25);
	//        sleep(1);
	//	}
	while(1)
	{
		status = MFRC522_Request(PICC_REQIDL, str);
		if(resulta==status){

		}else{
			resulta=status;
		}


		if (status == MI_OK)
		{
			mifare_tag_type(str);
			//usleep(1000);
		}

		status = MFRC522_Anticoll(str);
		memcpy(serNum, str,5);
		if (status == MI_OK)
		{
			printf("UID :");
			for(i=0;i<5;i++)
			{
				printf("[");
				printf("%02X",serNum[i]);
				printf("]");
			}
			printf("\n");
			//usleep(1000);
		}

		RC_size=MFRC522_SelectTag(serNum);
		if(RC_size !=0)
		{

			printf("Select_OK Size %02d \n",RC_size);
			puts("");
			puts("");

		}

	}

	close(fd);

}

unsigned char mifare_tag_type(unsigned char * b)
{
	puts("");
	puts("");
	if(b[0]==0x44 && b[1]==0x00 )
	{ printf("Mifare_UltraLight \n"); return 1;}

	if(b[0]==0x04 && b[1]==0x00 )
	{ printf("Mifare_One(S50) \n");  return 1;}

	if(b[0]==0x02 && b[1]==0x00 )
	{ printf("Mifare_One(S70) \n");  return 1;}

	if(b[0]==0x08 && b[1]==0x00 )
	{ printf("Mifare_Pro(X) \n");  return 1;}

	if(b[0]==0x44 && b[1]==0x03 )
	{ printf("Mifare_DESFire \n");  return 1;}
	return 0;
}


