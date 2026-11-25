/*
 * uart.c
 *
 * Created: 2026. 03. 26. 10:27:23
 *  Author: majer
 */ 
#include "uart.h"

/** @file uart.c
 *
 * @brief In this module, we configure the corresponding pins for the uart, and setting it up
 *in a one wire mode to communicate with the TMC2209 stepper motor driver.
 */

/*Using uart2*/

/*started programing on 26.03.2026*/


void uart_init(uint16_t baudrate)
{
	#define USART2_BAUD_RATE(baudrate) ((float)(4000000 * 64 / (16 * (float)baudrate)) + 0.5)

	//redirect USART2 to PF4,PF5 from PF0 and PF1
	PORTMUX.USARTROUTEA = PORTMUX_USART2_ALT1_gc;

	//Set up 1 wire communication
	USART2.CTRLA = (1 << USART_LBME_bp);
	USART2.CTRLB = (1 << USART_ODME_bp) | (1 << USART_RXEN_bp) | (1 << USART_TXEN_bp);
	USART2.CTRLC = USART_CHSIZE_8BIT_gc | USART_SBMODE_1BIT_gc;

	//Enable receive complete interrupt
	USART2.CTRLA |= (1 << USART_RXCIE_bp);

	USART2.BAUD = (uint16_t)USART2_BAUD_RATE(baudrate);
}

void uart_write(uint8_t data)
{
	while (!(USART2.STATUS & USART_DREIF_bm));
	USART2.TXDATAL = data;
}