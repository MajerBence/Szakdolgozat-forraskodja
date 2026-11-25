/*
 * encoder.c
 *
 * Created: 2026. 04. 09. 8:56:14
 *  Author: majer
 */ 
#include "encoder.h"
 
#define ENCODER_A	PIN0_bm
#define ENCODER_B	PIN1_bm
#define MAXIMUM_ENCODER_VALUE 99u

void encoder_init(void)
{
	PORTD.DIRCLR = ENCODER_A | ENCODER_B;
	PORTD.PINCONFIG = (1 << PORT_PULLUPEN_bp) |  (0 << PORT_INVEN_bp) ;		//MEGCSERÉLTEM A KETTÕT 0902 ÉS ÁTTETTEM 32DB48-RA!
	PORTD.PINCTRLUPD = ENCODER_A | ENCODER_B;
}

static inline uint8_t get_encoder_A()
{
	return (!(VPORTD.IN & ENCODER_A));
}

static inline uint8_t get_encoder_B()
{
	return (!(VPORTD.IN & ENCODER_B));
}

uint8_t convert_graycode_to_binary(void)
{
	uint8_t encoder_channel_A = get_encoder_A();
	uint8_t encoder_channel_B = get_encoder_B();

	if((encoder_channel_A == 0) && (encoder_channel_B == 0))
	{
		return 0;
	}
	else if ((encoder_channel_A == 0) && (encoder_channel_B == 1)) 
	{
		return 1;
	}
	else if ((encoder_channel_A == 1) && (encoder_channel_B == 1))
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

static volatile uint16_t rotation_counter = 0;	

void encoder_read_position(volatile uint8_t* ptr_encoder_position)
{
	static int8_t current_position	 = 0;
	static int8_t previous_position  = 0;
	

	int8_t difference = 0;

	current_position = convert_graycode_to_binary();
	difference = current_position - previous_position;

	if((difference == -1) || (difference == 3))
	{
		previous_position = current_position;
		rotation_counter++;
	}
	else if ((difference == 1) || (difference == -3)) 
	{
		previous_position = current_position;
		rotation_counter--;
	}
	else
	{
		previous_position = current_position;
	}

	//if we over go dec:99 we reset the rotation counter, and the rotary_number

	if(*ptr_encoder_position > MAXIMUM_ENCODER_VALUE)
	{
		*ptr_encoder_position = 0x00;
		rotation_counter = 0;
	}

	else
	{
		*ptr_encoder_position = rotation_counter >> 2;			//Divide by two
	}	
}

void encoder_flush_reading(void)
{
	rotation_counter = 0;
}

extern volatile uint8_t encoder_current_position;

ISR(TCB0_INT_vect)
{
	TCB0.INTFLAGS = TCB_CAPT_bm;    //delete interrupt flag
	encoder_read_position(&encoder_current_position);	
}