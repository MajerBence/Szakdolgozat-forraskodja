/*
 * tmc2209.c
 *
 * Created: 2026. 04. 03. 14:56:03
 *  Author: majer
 */ 

#include "tmc2209.h"

#include <util/delay.h>

#define  SYNC_DATA		0x05
#define  COMMAND_WRITE	0x80
#define  COMMAND_READ	~0x80
#define	 MASTER_ADDRES	0xFF

#define RX_BUFFER_SIZE	12u
volatile uint8_t recived_data_buffer[RX_BUFFER_SIZE] = {0};
volatile uint8_t rx_index = 0;


/*This CRC calculation is form the TCM datasheet*/
void swuart_calcCRC(uint8_t* datagram, uint8_t datagramLength)
{
	int i,j;
	uint8_t* crc = datagram + (datagramLength-1); // CRC located in last byte of message
	uint8_t currentByte;
	*crc = 0;
	for (i=0; i<(datagramLength-1); i++) { // Execute for all bytes of a message
		currentByte = datagram[i]; // Retrieve a byte to be sent from Array
		for (j=0; j<8; j++) {
			if ((*crc >> 7) ^ (currentByte&0x01)) // update CRC based result of XOR operation
			{
				*crc = (*crc << 1) ^ 0x07;
			}
			else
			{
				*crc = (*crc << 1);
			}
			currentByte = currentByte >> 1;
		} // for CRC bit
	} // for message byte
}

void tmc_write(uint8_t tmc_address, tmc2209_registers_t register_name, uint32_t data)
{
	uint8_t data_array[4] = {data >> 24, data >> 16, data >> 8, data};
	uint8_t forwarding_data[8] = 
	{
	SYNC_DATA, 
	tmc_address, 
	register_name | COMMAND_WRITE,
	data_array[0],
	data_array[1],
	data_array[2], 
	data_array[3], 
	0						//the last one is the crc
	};
	
	//Now we use the example code from the datasheet
	
	swuart_calcCRC(forwarding_data,8);	

	//For the final part we send the complete data!

	for(uint8_t i = 0; i <= 8-1; i++)
	{
		uart_write(forwarding_data[i]);
	}
}



uint32_t tmc_read(uint8_t tmc_address, tmc2209_registers_t register_name)
{	
	uint8_t forwarding_data[4] =
	{
		SYNC_DATA,
		tmc_address,
		register_name & COMMAND_READ,
		0						//the last one is the crc
	};
	uint32_t decoded_data = 0;
	rx_index = 0;
	
	swuart_calcCRC(forwarding_data,4);		//Now we use the example code from the data sheet to calculate the crc

	for(uint8_t i = 0; i <= 4-1; i++)
	{
		uart_write(forwarding_data[i]);
	}

	rx_index = 0;

	_delay_ms(20);
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if ((recived_data_buffer[3] == SYNC_DATA)		&& 
			(recived_data_buffer[4] == MASTER_ADDRES)	&& 
			(recived_data_buffer[5] == register_name))
		{
			decoded_data =	(uint32_t)recived_data_buffer[6] << 24 |
							(uint32_t)recived_data_buffer[7] << 16 |
							(uint32_t)recived_data_buffer[8] << 8  |
							(uint32_t)recived_data_buffer[9];
		}
	}
	return decoded_data;
}

//In this interrupt we manage the reading form the TMC

ISR(USART2_RXC_vect)
{
	uint8_t buffer_read = USART2.RXDATAL;		//we need to read the RXDATAL to clear interrupt flag!
	if(rx_index < RX_BUFFER_SIZE)
	{
		recived_data_buffer[rx_index] = USART2.RXDATAL;
		rx_index++;
	}
	
}