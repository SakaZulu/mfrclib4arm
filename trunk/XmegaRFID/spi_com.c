/*
 * spi_com.c
 *
 * Created: 28/04/2013 12:27:17 p.m.
 * Author: jlarevaloh@gmail.com
 * This file is based on Atmel´s SPI example code.
 */ 

#include "spi_com.h"
#include "spi_driver.h"

/*! \brief SPI master on PORT C. */
SPI_Master_t spiMasterC;

/*! \brief Data packet. */
//SPI_DataPacket_t dataPacket;

/*! \brief Test data to send. */
//const uint8_t sendData[NUM_BYTES + 1] = { 0x55, 0xaa, 0x00 };

/*! \brief Buffer for test data reception. */
//uint8_t receivedData[NUM_BYTES + 1];

/*! \brief Result of the example test. */
bool success;

void spi_com_init()
{
	/* Init SS pin as output with wired AND and pull-up. */
	PORTC.DIRSET = PIN4_bm;
	PORTC.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;

	/* Set SS output to high. (No slave addressed). */
	PORTC.OUTSET = PIN4_bm;

	/* Initialize SPI master on port C. */
	SPI_MasterInit(&spiMasterC,
	&SPIC,
	&PORTC,
	false,
	SPI_MODE_0_gc,
	SPI_INTLVL_LO_gc,
	false,
	SPI_PRESCALER_DIV4_gc);

	/* Enable low and medium level interrupts in the interrupt controller. */
	PMIC.CTRL |= PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	//sei();	
}
	
/*! \brief SPI master interrupt service routine.
 *
 *  The interrupt service routines calls one common function,
 *  SPI_MasterInterruptHandler(SPI_Master_t *spi),
 *  passing information about what module to handle.
 *
 *  Similar ISRs must be added if other SPI modules are to be used.
 */
ISR(SPIC_INT_vect)
{
	SPI_MasterInterruptHandler(&spiMasterC);
}

