/*
 * spi.h
 *
 * Created: 2026. 04. 09. 12:09:52
 *  Author: majer
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
void spi_init(void);
void spi_write(uint8_t data);

#endif /* SPI_H_ */