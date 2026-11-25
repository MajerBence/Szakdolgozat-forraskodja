/*
 * tmc2209.h
 *
 * Created: 2026. 04. 03. 14:55:22
 *  Author: majer
 */ 


#ifndef TMC2209_H_
#define TMC2209_H_

#include <xc.h>
#include <stdint.h>
#include <util/atomic.h>

#include "uart.h"

typedef enum 
{
	/*
	*	To know what each register is doing see the datasheet at page 22 --> Register Map
	*/

	//General Registers//

	/*datasheet -> page 23.*/

	GCONF		= 0x00,		//Global configuration flags							RW
	GSTAT		= 0x01,		//Global status flags									R+WC
	IFCNT		= 0x02,		//Interface transmission counter						R
	NODECONF	= 0x03,		//SENDDELAY for read access								W
	IOIN		= 0x06,		//INPUT (Reads the state of all input pins available)	R

	//StallGuard Control//

	/*datasheet -> page 29.*/

	TCOOLTHRS	= 0x14,		//StallGuard enable register							W
	SGTHRS		= 0x40,		//StallGuard threshold									W
	SG_RESULT	= 0x41,		//StallGuard result register							R

	//Sequencer Registers//

	/*datasheet -> page 31.*/
	
	MSCNT		= 0x6A,		//Microstep counter										R
	MSCURACT	= 0x6B,		//Actual microstep current for motor phase				R

	//Chopper Configuration

	/*datasheet -> page 33*/

	CHOPCONF	= 0x6C

	

}tmc2209_registers_t;


void tmc_write(uint8_t tmc_address, tmc2209_registers_t register_name, uint32_t data);
uint32_t tmc_read(uint8_t tmc_address, tmc2209_registers_t register_name);

#endif /* TMC2209_H_ */