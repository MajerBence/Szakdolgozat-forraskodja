/*
 * motor.h
 *
 * Created: 2026. 04. 27. 13:35:28
 *  Author: majer
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdbool.h>

#include "encoder.h"
#include "display.h"
#include "user.h"
#include "timer.h"

void motor_init(void);
void default_position_led(void);

inline void motor_pulsegeneration_start(void)
{
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

inline void motor_pulsegeneration_stop(void)
{
	TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
	PORTE.OUTCLR = PIN0_bm;
}

void motor_start_operation(void);

#endif /* MOTOR_H_ */