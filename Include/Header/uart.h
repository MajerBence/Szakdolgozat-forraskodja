/*
 * uart.h
 *
 * Created: 2026. 03. 26. 10:27:09
 *  Author: majer
 */ 

#include <xc.h>
#include <stdio.h>

#ifndef UART_H_
#define UART_H_
#include <avr/interrupt.h>

void uart_init(uint16_t baudrate);
void uart_write(uint8_t data);
#endif /* UART_H_ */