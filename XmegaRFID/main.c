
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

/* The number of coroutines to create. */
#define mainNUM_FLASH_COROUTINES		( 3 )

/*
 * The idle hook is used to scheduler co-routines.
 */
void vApplicationIdleHook( void );
static void task1( void *pvParameters );
static void task2( void *pvParameters );

unsigned char gFlag;

int main(void)
{
	leds_init();
	osc_intern_32MHz_init();

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

	for( ;; )
	{
		gFlag ^= (1<<0);
		vTaskDelay(500);
	}
}

