/*
 * display.c
 *
 * Created: 2026. 04. 10. 10:42:39
 *  Author: majer
 */ 

#include "display.h"

//MODIFIED 07.01
const uint8_t number[] = {
	0b00000011, // 0
	0b10011111, // 1
	0b00100101, // 2
	0b00001101, // 3
	0b10011001, // 4
	0b01001001, // 5
	0b01000001, // 6
	0b00011111, // 7
	0b00000001, // 8
	0b00001001  // 9
};

#define SEG0	PIN3_bm
#define SEG1	PIN2_bm

void display_init(void)
{
	PORTA.DIRSET = SEG0 | SEG1;

	PORTA.PIN2CTRL = PORT_INVEN_bm;
	PORTA.PIN3CTRL = PORT_INVEN_bm;
	spi_init();
}

void display_position_select(uint8_t segment_select)
{
	if(segment_select == 0)
	{
		VPORTA.OUT &= ~SEG0;
		VPORTA.OUT |= SEG1;
	}
	else if(segment_select == 1)
	{
		VPORTA.OUT |= SEG0;
		VPORTA.OUT &= ~SEG1;
	}
	else;
}

void display_number(uint8_t displaying_number)
{
	uint8_t segment[2] = {0};

	segment[0] = (displaying_number / 10) % 10;
	segment[1] =  displaying_number % 10;

	for(int i = 0; i <= 1; i++){
		spi_write(number[segment[i]]);
		display_position_select(i);
		_delay_us(10);
	}
}

void display_turn_off(void)
{
	PORTA.OUTCLR = SEG0 | SEG1;
}