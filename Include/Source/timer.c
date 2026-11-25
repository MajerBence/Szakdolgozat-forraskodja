/*
 * timer.c
 *
 * Created: 2026. 04. 09. 11:34:54
 *  Author: majer
 */ 
#include "timer.h"

void timer_encoder_overflow_init(void)
{	
	TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | (0 << TCB_ENABLE_bp);
	TCB0.CTRLB = TCB_CNTMODE_INT_gc;
	TCB0.INTCTRL = TCB_CAPT_bm;

	TCB0.CCMP = 23000;
	TCB0.CTRLA |= (1 << TCB_ENABLE_bp);
}

void timer_motor_pulse_generation_init(void)
{
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTE_gc;			//Reroute TCA w0 from PortA to PortE
	PORTE.DIRSET = PIN0_bm;

	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc;							//div2 to div64
	TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_FRQ_gc;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
	TCA0.SINGLE.INTCTRL &= ~1 << (TCA_SINGLE_OVF_bp);

	TCA0.SINGLE.CMP0 = 10;		//motor speed control			eredit 100!!!!
}