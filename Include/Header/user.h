/*
 * button.h
 *
 * Created: 2026. 04. 26. 13:34:53
 *  Author: majer
 */ 


#ifndef USER_H_
#define USER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>


//Buttons are located on PORTD

#define BTN_STOP	PIN2_bm
#define BTN_START	PIN3_bm
#define BTN_LATCH	PIN4_bm

//The leds are located on PORTD
#define LED_STOP		PIN7_bm		//green
#define LED_LATCH		PIN6_bm		//yellow
#define LED_RUN			PIN5_bm		//red


void user_interface_init(void);

volatile uint8_t pushbutton_value;
volatile uint8_t pushbutton_value_copy;

static inline bool is_start_pushbutton_pressed()
{
	pushbutton_value_copy = pushbutton_value;
	pushbutton_value &= ~BTN_START;
	return ((pushbutton_value_copy & BTN_START) != 0);
}

static inline bool is_stop_pushbutton_pressed()
{
	pushbutton_value_copy = pushbutton_value;
	pushbutton_value &= ~BTN_STOP;
	return ((pushbutton_value_copy & BTN_STOP) != 0);
}

static inline bool is_latch_pushbutton_pressed()
{
	pushbutton_value_copy = pushbutton_value;
	pushbutton_value &= ~BTN_LATCH;
	return ((pushbutton_value_copy & BTN_LATCH) != 0);
}





static inline bool is_user_green_led_on(void)
{
	return (VPORTD.IN & LED_RUN) != 0;
}
static inline bool is_user_red_led_on(void)
{
	return (VPORTD.IN & LED_STOP) != 0;
}
static inline bool is_user_yellow_led_on(void)
{
	return (VPORTD.IN & LED_LATCH) != 0;
}


static inline void user_green_led_on(void)
{
	VPORTD.OUT |= LED_RUN;
}
static inline void user_green_led_off(void)
{
	VPORTD.OUT &= ~LED_RUN;
}


static inline void user_red_led_on(void)
{
	VPORTD.OUT |= LED_STOP;
}
static inline void user_red_led_off(void)
{
	VPORTD.OUT &= ~LED_STOP;
}

static inline void user_yellow_led_on(void)
{
	VPORTD.OUT |= LED_LATCH;
}
static inline void user_yellow_led_off(void)
{
	VPORTD.OUT &= ~LED_LATCH;
}



static inline void user_green_led_toggle(void)
{
	VPORTD.OUT ^= LED_RUN;
}
static inline void user_red_led_toggle(void)
{
	VPORTD.OUT ^= LED_STOP;
}
static inline void user_yellow_led_toggle(void)
{
	VPORTD.OUT ^= LED_LATCH;
}
#endif /* BUTTON_H_ */