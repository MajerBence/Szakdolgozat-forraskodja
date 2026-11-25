/*
 * motor.c
 *
 * Created: 2026. 04. 27. 13:35:18
 *  Author: majer
 */ 
#include "motor.h"


/*These two are for the feedback, if the motor stalls, the diagnostic pin will give a pulse that something went wrong 
* if the index pin doesn't give pulses, while we clearly send out pulses to drive the motor,
* we also give an error, that something went wrong!
*/

/* case 1.
*	We drive the motor with the step pin, and we can read back pulses from the index pin, and read back 
*	0V from the diagnostic pin, then everything is good.
*
*  case 2.
*	We drive the motor with the step pin, but we can't read back pulses from the index pin, so there is a problem, 
*	and we stop the program execution .
*
*  case 3.
*	We drive the motor with the step pin,and we can read back pulses from the index pin but we get a pulse on 
*	the diagnostic pin, theres is also a problem, so we stop the program execution.
*/

//These are located on PORTE

/*Defines*/
#define MOTOR_PULSE			PIN0_bm
#define MOTOR_ENABLE		PIN1_bm
#define MOTOR_DIAGNOSTIC	PIN2_bm
#define MOTOR_INDEX			PIN3_bm
	
#define NUMBER_OF_STEPS_FOR_A_FULL_REVOLUTION 614400UL		
#define MAXIMUM_DIVISON_NUMBER		99U
/*Defines*/


/*Variables*/
volatile uint8_t encoder_current_position			= 0;
volatile uint8_t motor_step_index					= 0;
volatile uint8_t sent_steps_finished_flag			= 0;
volatile uint32_t completed_steps					= 0;
volatile uint16_t step_counter_for_error_handler	= 0;
volatile uint8_t feedback_flag						= 0;
/*Variables*/


/*Typedef*/
typedef enum
{
	WAIT,
	RUN
}stepper_motor_state_t;

typedef struct
{
	int32_t base_step;
	int32_t step_remainder;
	int32_t error_accumulation;
	uint32_t calculated_steps[MAXIMUM_DIVISON_NUMBER];		//maximum divison number
}stepper_motor_calculations_t;

stepper_motor_calculations_t motor;
/*Typedef*/


void motor_init(void)
{
	PORTE.DIRSET = MOTOR_PULSE | MOTOR_ENABLE;				//Set to output
	PORTE.DIRCLR = MOTOR_DIAGNOSTIC | MOTOR_INDEX;			//Set to input

	PORTE.OUTSET = MOTOR_ENABLE;
	PORTE.OUTCLR = MOTOR_PULSE;

	PORTE.PIN3CTRL = PORT_ISC_RISING_gc;					//Enable interrupt for index pin
	PORTE.PIN2CTRL = PORT_ISC_RISING_gc;					//Enable interrupt for diagnostic pin
}

inline void motor_latch_enable(void)
{
	PORTE.OUTCLR = MOTOR_ENABLE;					
}

inline void motor_latch_dissable(void)
{
	PORTE.OUTSET = MOTOR_ENABLE;					
}

inline void motor_latch_toggle(void)
{
	PORTE.OUTTGL = MOTOR_ENABLE;					
}


void operation_started(void)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		user_red_led_toggle();
		_delay_ms(200);
	}
}

void operation_finished(void)
{
	user_green_led_on();
	user_red_led_on();
	user_yellow_led_on();

	for (uint8_t i = 0; i < 5; i++)
	{
		user_green_led_toggle();
		user_red_led_toggle();
		user_yellow_led_toggle();
		_delay_ms(500);
	}
	user_green_led_on();
	user_yellow_led_on();
}

void operation_ended_with_error(void)
{
	for (uint8_t i = 0; i < 13; i++)
	{
		user_red_led_toggle();
		_delay_ms(300);
	}
	user_green_led_on();
	user_yellow_led_on();
	user_red_led_off();
}


//This is used, in the first power up
void default_position_led(void)
{
	user_green_led_on();
	user_yellow_led_off();
	user_red_led_off();
	motor_latch_dissable();
}
void run_position_led(void)
{
	user_green_led_off();
	user_red_led_on();
}
void waite_position_led(void)
{
	user_green_led_on();
	user_red_led_off();
}

//This only runs once!
void calculate_stepper_motor_steps(void)
{
	uint8_t division = encoder_current_position;			
	
	motor.base_step = NUMBER_OF_STEPS_FOR_A_FULL_REVOLUTION / division;
	motor.step_remainder = NUMBER_OF_STEPS_FOR_A_FULL_REVOLUTION % division;

	motor.error_accumulation = (motor.step_remainder << 1) - division;

	for (uint8_t i = 0; i < division; i++)
	{
		if (motor.error_accumulation > 0)
		{
			motor.error_accumulation += (motor.step_remainder - division) << 1;	//multiply by 2
			motor.calculated_steps[i] = motor.base_step + 1;
		}
		else
		{
			motor.error_accumulation += motor.step_remainder << 1;				//multiply by 2
			motor.calculated_steps[i] = motor.base_step;
		}
	}
}

void calculate_stepper_motor_steps_clear(void)
{
	motor.base_step				= 0;
	motor.error_accumulation	= 0;
	motor.step_remainder		= 0;

	for (uint8_t i = 0; i < MAXIMUM_DIVISON_NUMBER; i++)
	{
		motor.calculated_steps[i] = 0;
	}
}

bool motor_error_handler(void)
{
	//If an error occur or not, we reset the feedback signals back to false!

	//modisítva >= --> == 11.16
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (step_counter_for_error_handler >= 2048 && (feedback_flag & MOTOR_INDEX))
		{
			feedback_flag &= ~MOTOR_INDEX;
			step_counter_for_error_handler = 0;
			return false;
		}
		else if ((step_counter_for_error_handler >= 2048) && (!(feedback_flag & MOTOR_INDEX)))
		{
			feedback_flag &= ~MOTOR_INDEX;
			step_counter_for_error_handler = 0;
			return true;
		}

		if (feedback_flag & MOTOR_DIAGNOSTIC)
		{
			feedback_flag &= ~MOTOR_DIAGNOSTIC;
			step_counter_for_error_handler = 0;
			return true;
		}
	}
	return false;
}


void motor_start_operation(void)
{
	stepper_motor_state_t motor_state = WAIT;
	uint8_t motor_display_current_step = 1;
	bool error_occured = false;
	
	motor_step_index = 0;
    completed_steps = 0;
    sent_steps_finished_flag = 0;
	step_counter_for_error_handler = 0;										//11.23!

	//Select the dividing number with the encoder, and display it
	do
	{
		if (is_latch_pushbutton_pressed())
		{
			user_yellow_led_toggle();
			motor_latch_toggle();
		}
		else
		{
			display_number(encoder_current_position);
		}
	}
	while ((!is_start_pushbutton_pressed()) || (encoder_current_position == 0));	//doesn't go to next stage if we want to divide by zero

	operation_started();

	if (!is_user_yellow_led_on())
	{
		user_yellow_led_on();
		motor_latch_enable();
	}
	
	calculate_stepper_motor_steps();

	do 
	{
		if (is_stop_pushbutton_pressed())
		{
			motor_pulsegeneration_stop();
			error_occured = true;
			break;
		}

		switch(motor_state)
		{
			case(WAIT):
				
				display_number(motor_display_current_step);
				if (is_start_pushbutton_pressed())
				{
					motor_state = RUN;
					run_position_led();
					display_turn_off();
					motor_pulsegeneration_start();
				}
				break;
			case(RUN):
				
				if(motor_error_handler())
				{
					motor_pulsegeneration_stop();
					error_occured = true;
					break;
				}

				if (completed_steps == motor.calculated_steps[motor_step_index])
				{
					motor_pulsegeneration_stop();
					sent_steps_finished_flag = 1;
				}

				if (sent_steps_finished_flag)		
				{
					waite_position_led();
					motor_state = WAIT;	
					motor_step_index++;
					motor_display_current_step++;

					completed_steps = 0;
					sent_steps_finished_flag = 0;
				}
				
				break;
		}	
	}
	while ((motor_step_index != encoder_current_position) && !error_occured);

	if (error_occured)
	{
		operation_ended_with_error();
	}
	else
	{
		operation_finished();
	}

	encoder_flush_reading();
	motor_step_index = 0;
	completed_steps = 0;
	error_occured = false;
}





ISR(TCA0_CMP0_vect)
{
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
	completed_steps++;
	//step_counter_for_error_handler++;
	
}

//Feedback from the driver

ISR(PORTE_PORT_vect)
{	
	 if (VPORTE.INTFLAGS & MOTOR_INDEX) {
		 step_counter_for_error_handler++;
	 }
	feedback_flag |= VPORTE.INTFLAGS & (MOTOR_DIAGNOSTIC | MOTOR_INDEX);
	VPORTE.INTFLAGS = MOTOR_DIAGNOSTIC | MOTOR_INDEX;	
}