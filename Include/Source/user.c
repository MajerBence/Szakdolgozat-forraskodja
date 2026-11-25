/*
 * button.c
 *
 * Created: 2026. 04. 26. 13:34:42
 *  Author: majer
 */ 

#include "user.h"
#define BTN_VALUE_FLUSH 0x1Cu

//Set PORTD 2,3,4 pins as inputs, enable pull up resistors and generate interrupt if its connected to gnd

void user_interface_init(void)
{
	//Button setup
	PORTD.DIRCLR = BTN_START | BTN_STOP | BTN_LATCH;
	//TODO may need to rethink how we read the button!
	PORTD.PINCONFIG =  PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;
	PORTD.PINCTRLUPD = BTN_START | BTN_STOP | BTN_LATCH;

	//Led setup
	PORTD.DIRSET =  LED_STOP | LED_LATCH | LED_RUN;
}


ISR(PORTD_PORT_vect)
{
	pushbutton_value = VPORTD.INTFLAGS;
	VPORTD.INTFLAGS = BTN_VALUE_FLUSH;
}