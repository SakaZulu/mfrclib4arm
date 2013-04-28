
/*
 * main.c
 *
 * Created: 27/04/2013 06:45:19 p.m.
 * Author: jlarevaloh@gmail.com
 * Hackaton!, http://www.hackathonmty.org/
 * Description: This program shows how to implement the FreeRTOS 
 * on an ATxmega128A1 microcontroller (Xmega-A1 Xplained Atmel´s board).
 */ 

#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "leds.h"
#include "clksys_driver.h"
#include "spi_driver.h"
#include "spi_com.h"

/* The number of coroutines to create. */
#define mainNUM_FLASH_COROUTINES		( 3 )

/*
 * The idle hook is used to scheduler co-routines.
 */
void vApplicationIdleHook( void );
static void task1( void *pvParameters );
static void task2( void *pvParameters );

unsigned char gFlag;
extern SPI_Master_t spiMasterC;

int main(void)
{
	leds_init();
	osc_intern_32MHz_init();
	spi_com_init();

	gFlag = 0;
	xTaskCreate( task1, ( signed char * ) "Task1", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY+2), ( xTaskHandle * )NULL );
	xTaskCreate( task2, ( signed char * ) "Task2", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY+2), ( xTaskHandle * )NULL );
	
	//Enables low priority interrupts!
	PMIC.CTRL |= PMIC_LOLVLEN_bm;// | PMIC_HILVLEN_bm;
	
	vTaskStartScheduler();

	return 0;	
}

void vApplicationIdleHook( void )
{
}

static portTASK_FUNCTION( task1, pvParameters )
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	for( ;; )
	{
		if(gFlag & (1<<0)){
			leds_pwr_tgl();
		}
		else{
			leds_pwr_off();
		}
		vTaskDelay(50);
	}
} 

static portTASK_FUNCTION( task2, pvParameters )
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	SPI_DataPacket_t myDataPacket;
	#define buff_len	3
	unsigned char buff_tx[buff_len];
	unsigned char buff_rx[buff_len];
	buff_tx[0] = 0x33;
	buff_tx[1] = 0x33;
	buff_tx[2] = 0x55;

	/* Create data packet */
	SPI_MasterCreateDataPacket(
	&myDataPacket,
	buff_tx,
	buff_rx,
	buff_len,
	&PORTC,
	PIN4_bm);
	
	unsigned char cTemp;
			
	for( ;; )
	{
		gFlag ^= (1<<0);
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
			
		/* Transmit and receive first data byte. */
		uint8_t status;
		do {
			status = SPI_MasterInterruptTransceivePacket(&spiMasterC, &myDataPacket);
			vTaskDelay(1);
		} while (status != SPI_OK);
		/* Wait for transmission to complete. */
		while(myDataPacket.complete == false) {
			vTaskDelay(1);
		}
		
		cTemp = spiMasterC.module->DATA;
		cTemp = 0x00;
		buff_tx[2]++;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
		vTaskDelay(500);
	}
}

