/*
 * display.h
 *
 * Created: 2026. 04. 10. 10:42:45
 *  Author: majer
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"

void display_init(void);
void display_number(uint8_t displaying_number);
void display_turn_off(void);
#endif /* DISPLAY_H_ */