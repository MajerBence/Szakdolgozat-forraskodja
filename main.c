/*
 * main.c
 *
 * Created: 3/26/2026 10:22:53 AM
 *  Author: majer
 */ 

//F_CPU is defined globally!
#include <avr/io.h>
#include <util/delay.h>

#include "motor.h"
#include "tmc2209.h"
#include "user.h"

//////////////////////////////////////////////////////////////////////////
uint32_t microstep_set_from_softwer  = 0x0181;			//385
uint32_t native_256_microstep_enable = 0x10010053;		//268501075
//////////////////////////////////////////////////////////////////////////
uint32_t kacsi = 0;
volatile uint16_t test_stallguard = 0;

extern uint8_t encoder_current_position;

int main(void)
{
	//Disable global interrupts
	cli();

	//User interface setup
	user_interface_init();	
	encoder_init();
	timer_encoder_overflow_init();

	//Display setup
	spi_init();
	display_init();

	//Motor driver setup
	uart_init(9600);			
	motor_init();
	timer_motor_pulse_generation_init();

	//Enable global interrupt
	sei();



	//Write in the required parameters to the driver


	//Micro step resolution is selected by MRES register instead of MS1,MS2 physical pins
	tmc_write(0x00,GCONF,microstep_set_from_softwer);
	_delay_ms(100);
	tmc_write(0x00,CHOPCONF,native_256_microstep_enable);
	_delay_ms(100);
	//Set the stall detection window!
	tmc_write(0x00,TCOOLTHRS,2000);		//????
	_delay_ms(100);										//most lett hozzáadva 1114!
	tmc_write(0x00,SGTHRS,50);
	_delay_ms(100);
	

	default_position_led();
    
	user_interface_init();
	
	while(1)
    {	

		//_delay_ms(1000);
		motor_start_operation();
	}
	return 0;
}
