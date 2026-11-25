/*
 * spi.c
 *
 * Created: 2026. 04. 09. 12:09:43
 *  Author: majer
 */ 
#include "spi.h"
#define SPI_DATA	PIN4_bm
#define SPI_CLK		PIN6_bm
#define SPI_SS		PIN7_bm

void spi_init(void)
{
	//Set up the port for the SPI
	PORTA.DIRSET = SPI_DATA | SPI_CLK | SPI_SS;

	PORTA.PIN7CTRL = PORT_INVEN_bm;

	/*SPI peripheral set up*/

	SPI0.CTRLA &= ~(1 << SPI_ENABLE_bp);	//Disable SPI

	SPI0.CTRLA = (1 << SPI_DORD_bp) | (1 << SPI_MASTER_bp) | SPI_PRESC_DIV16_gc;
	SPI0.CTRLB = (0 << SPI_SSD_bp) | SPI_MODE_0_gc;

	SPI0.CTRLA |= SPI_ENABLE_bm;			//Enable SPI
}

static inline void spi_ss_toggle(void)
{
	PORTA.OUTTGL = SPI_SS;
	_delay_ms(1);
	PORTA.OUTTGL = SPI_SS;
}

void spi_write(uint8_t data)
{
	SPI0.DATA = data;
	while (!(SPI0.INTFLAGS & SPI_IF_bm));
	spi_ss_toggle();
}