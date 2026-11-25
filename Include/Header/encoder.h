/*
 * encoder.h
 *
 * Created: 2026. 04. 09. 8:56:30
 *  Author: majer
 */ 


#ifndef ENCODER_H_
#define ENCODER_H_

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdbool.h>



void encoder_init(void);
void encoder_read_position(volatile uint8_t* ptr_encoder_position);
void encoder_flush_reading(void);


#endif /* ENCODER_H_ */