/*
 * I2C.c
 *
 * Created: 22-9-2017 08:48:32
 *  Author: adri
 */ 
#include <Wire.h>
#include "I2C.h"


void I2C_init (void)
{
	Wire.begin();
}


void I2C_write_register (uint8_t slaveAddress, uint8_t Register, uint8_t data)
{
	Wire.beginTransmission(slaveAddress);
	Wire.write(Register);
	Wire.write(data);
	Wire.endTransmission();
}


void I2C_write_array (uint8_t slaveAddress, uint8_t Register, uint8_t *data, uint8_t lenght)
{
	Wire.beginTransmission(slaveAddress);
	Wire.write(Register);
	Wire.write(data, lenght);
	Wire.endTransmission();	
}



uint8_t I2C_read_register (uint8_t slaveAddress, uint8_t Register)
{
	uint8_t retVal;
	
	Wire.beginTransmission(slaveAddress);
	Wire.write(Register);
	Wire.endTransmission();	
	Wire.requestFrom(slaveAddress, (uint8_t)1);
	retVal = Wire.read();
	Wire.endTransmission();
	return retVal;
}


void I2C_read_array (uint8_t slaveAddress, uint8_t Register, uint8_t *data, uint8_t lenght)
{
	uint8_t retVal, i;
	
	Wire.beginTransmission(slaveAddress);
	Wire.write(Register);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)slaveAddress, lenght);
	
	// Read until all bytes have been retrieved.
	for( i = 0 ; i < lenght ; i++)
	{
		data[i] = Wire.read();	
	}
	
	Wire.endTransmission();
}
