
#include "avr_compiler.h"

#define LED_PWR		PIN0_bp	//PORTE
#define LED_RXD		PIN1_bp	//PORTE
#define LED_TXD		PIN2_bp	//PORTE
#define LED_TST		PIN3_bp	//PORTE

//Function prototype

void leds_init		(void);

void leds_rxd_off	(void);
void leds_txd_off	(void);

void leds_rxd_tgl	(void);
void leds_txd_tgl	(void);

void leds_pwr_tgl	(void);
void leds_pwr_off	(void);
void leds_pwr_on	(void);

void leds_tst_on	(void);
void leds_tst_off	(void);
