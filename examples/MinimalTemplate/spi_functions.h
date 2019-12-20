#ifndef _SPI_Functions_h
#define _SPI_Functions_h

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "Arduino.h"
	#else
		#include "WProgram.h"
	#endif

	#include <stdint.h>

	void	SPI_Write (uint8_t CS_pin, uint8_t register_Address, uint8_t Data);
	uint8_t SPI_Read (uint8_t CS_pin, uint8_t register_Address);
	void	SPI_Write_Array	(uint8_t CS_pin, uint8_t register_Address, uint8_t *Data,	uint8_t lenght);
	void	SPI_Read_Array (uint8_t CS_pin, uint8_t register_Address, uint8_t *Data, uint8_t lenght);

#endif
