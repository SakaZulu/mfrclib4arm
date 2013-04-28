
#include "leds.h"

void leds_init()
{
	//Habilita los leds 0,1 y 2
	PORTE.DIRSET = (1<<LED_PWR) | (1<<LED_RXD) | (1<<LED_TXD) | (1<<LED_TST);	
	PORTE.OUTCLR = (1<<LED_PWR);
	PORTE.OUTSET = (1<<LED_RXD) | (1<<LED_TXD) | (1<<LED_TST);		
}

void leds_rxd_off()
{
	PORTE.OUTSET = (1<<LED_RXD);
}

void leds_rxd_tgl()
{
	PORTE.OUTTGL = (1<<LED_RXD);
}
	
void leds_txd_off(void)
{
	PORTE.OUTSET = (1<<LED_TXD);
}

void leds_txd_tgl()
{
	PORTE.OUTTGL = (1<<LED_TXD);
}

void leds_pwr_tgl()
{
	PORTE.OUTTGL = (1<<LED_PWR);
}

void leds_pwr_off	(void)
{
	PORTE.OUTSET = (1<<LED_PWR);
}

void leds_pwr_on	(void)
{
	PORTE.OUTCLR = (1<<LED_PWR);
}

void leds_tst_on()
{
	PORTE.OUTCLR = (1<<LED_TST);
}

void leds_tst_off()
{
	PORTE.OUTSET = (1<<LED_TST);
}
