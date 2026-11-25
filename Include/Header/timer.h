/*
 * timer.h
 *
 * Created: 2026. 04. 09. 11:35:05
 *  Author: majer
 */ 


#ifndef TIMER_H_
#define TIMER_H_
#include <avr/io.h>

void timer_encoder_overflow_init(void);
void timer_motor_pulse_generation_init(void);
void timer_motor_diagnostic_init(void);

#endif /* TIMER_H_ */