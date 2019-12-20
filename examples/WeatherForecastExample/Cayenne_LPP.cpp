/*
 * Cayenne_LPP.c
 *
 * Created: 4-10-2017 11:45:10
 *  Author: adri
 */ 
#include <string.h>
#include "Cayenne_LPP.h"


CayenneLPP::CayenneLPP(sLoRa_Message *buffer_Ptr)
{
	if(buffer_Ptr != 0)
	{
		buffer = buffer_Ptr;
	}	
}

CayenneLPP::~CayenneLPP()
{
	buffer = 0;
}


void CayenneLPP::clearBuffer (void)
{
	if(buffer == 0)
	{
		return;
	}
	
	// Clear the buffer and the number of bytes in it.
	memset(&(buffer->Data[0]), 0, LORA_FIFO_SIZE);
	buffer->Count = 0;
}


void CayenneLPP::addGPS(uint8_t channel, double latitude, double longitude, double altitude)
{
	if((buffer == 0) || ((buffer->Count + LPP_GPS_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
	
	int32_t lat = (int32_t)(latitude  * 10000.0);
	int32_t lon = (int32_t)(longitude * 10000.0);
	int32_t alt = (int32_t)(altitude  * 100.0);
	
	buffer->Data[(buffer->Count) +  0] = channel;
	buffer->Data[(buffer->Count) +  1] = LPP_GPS;
	buffer->Data[(buffer->Count) +  2] = lat >> 16;
	buffer->Data[(buffer->Count) +  3] = lat >> 8;
	buffer->Data[(buffer->Count) +  4] = lat;
	buffer->Data[(buffer->Count) +  5] = lon >> 16;
	buffer->Data[(buffer->Count) +  6] = lon >> 8;
	buffer->Data[(buffer->Count) +  7] = lon;
	buffer->Data[(buffer->Count) +  8] = alt >> 16;
	buffer->Data[(buffer->Count) +  9] = alt >> 8;
	buffer->Data[(buffer->Count) + 10] = alt;
	buffer->Count += LPP_GPS_SIZE; // 11
	return;
}


void CayenneLPP::addAnalogOutput(uint8_t channel, double value) // sLoRa_Message *buffer, 
{
	int16_t val;
	if((buffer == 0) || ((buffer->Count + LPP_DIGITAL_OUTPUT_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
		
	val = (int16_t) (value * 100.0);
	
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_ANALOG_OUTPUT;
	buffer->Data[(buffer->Count) + 2] = val >> 8;
	buffer->Data[(buffer->Count) + 3] = val;
	buffer->Count += LPP_ANALOG_OUTPUT_SIZE; // 4
	return;
}


void CayenneLPP::addDigitalOutput(uint8_t channel, uint8_t value)
{
	// Check for invalid input pointer.
	if((buffer == 0) || ((buffer->Count + LPP_DIGITAL_OUTPUT_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
	
	// Add the digital value to the Transmit buffer.
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_DIGITAL_OUTPUT;
	buffer->Data[(buffer->Count) + 2] = value;
	
	// Add the added length to the buffer counter.
	buffer->Count += LPP_DIGITAL_OUTPUT_SIZE;
}



void CayenneLPP::addDigitalInput(uint8_t channel, uint8_t value) 
{
	// Check for invalid input pointer.
	if((buffer == 0) || ((buffer->Count + LPP_DIGITAL_INPUT_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
		
	// Add the digital value to the Transmit buffer.
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_DIGITAL_INPUT;
	buffer->Data[(buffer->Count) + 2] = value;
	buffer->Count += LPP_DIGITAL_INPUT_SIZE;
}



void CayenneLPP::addAnalogInput(uint8_t channel, float value) 
{
	uint16_t val;
	
	// Check for invalid input pointer.
	if((buffer == 0) || ((buffer->Count + LPP_ANALOG_INPUT_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
	
	val = (uint16_t) (value * 100.0);
		
	// Add the digital value to the Transmit buffer.
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_ANALOG_INPUT_SIZE;
	buffer->Data[(buffer->Count) + 2] = val >> 8;
	buffer->Data[(buffer->Count) + 3] = val >> 0;
	buffer->Count += LPP_ANALOG_INPUT_SIZE;
}



void CayenneLPP::addLuminosity(uint8_t channel, float lux) 
{
		uint16_t val;
		
		// Check for invalid input pointer.
		if((buffer == 0) || ((buffer->Count + LPP_LUMINOSITY_SIZE) > LORAWAN_MAX_PAYLOAD))
		{
			return;
		}
		
		val = (uint16_t) lux;
		
		// Add the digital value to the Transmit buffer.
		buffer->Data[(buffer->Count) + 0] = channel;
		buffer->Data[(buffer->Count) + 1] = LPP_ANALOG_INPUT_SIZE;
		buffer->Data[(buffer->Count) + 2] = val >> 8;
		buffer->Data[(buffer->Count) + 3] = val >> 0;
		buffer->Count += LPP_LUMINOSITY_SIZE;
}

void CayenneLPP::addPresence(uint8_t channel, uint8_t value) 
{	
	// Check for invalid input pointer.
	if((buffer == 0) || ((buffer->Count + LPP_PRESENCE_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
						
	// Add the digital value to the Transmit buffer.
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_PRESENCE;
	buffer->Data[(buffer->Count) + 2] = value;
	buffer->Count += LPP_PRESENCE_SIZE;
}

void CayenneLPP::addTemperature(uint8_t channel, float celsius) 
{
	uint16_t temp;
	
	// Check for invalid input pointer.
	if((buffer == 0) || ((buffer->Count + LPP_TEMPERATURE_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
	
	temp = (uint16_t) (celsius * 10.0);
		
	// Add the digital value to the Transmit buffer.
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_TEMPERATURE;
	buffer->Data[(buffer->Count) + 2] = temp >> 8;
	buffer->Data[(buffer->Count) + 3] = temp >> 0;
	buffer->Count += LPP_TEMPERATURE_SIZE;
}

void CayenneLPP::addRelativeHumidity(uint8_t channel, float rh) //sLoRa_Message *buffer, 
{
	uint16_t humidity;
		
	// Check for invalid input pointer.
	if((buffer == 0) || ((buffer->Count + LPP_RELATIVE_HUMIDITY_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
		
	humidity = (uint16_t) (rh * 2.0);
		
	// Add the digital value to the Transmit buffer.
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_RELATIVE_HUMIDITY;
	buffer->Data[(buffer->Count) + 2] = humidity;
	buffer->Count += LPP_RELATIVE_HUMIDITY_SIZE;
}

void CayenneLPP::addAccelerometer(uint8_t channel, float x, float y, float z) 
{
	uint16_t x_axis, y_axis, z_axis;
		
	// Check for invalid input pointer.
	if((buffer == 0) || ((buffer->Count + LPP_ACCELEROMETER_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
	
	// Convert the floats to unsigned integers.	
	x_axis = (uint16_t) (x * 1000.0);
	y_axis = (uint16_t) (y * 1000.0);
	z_axis = (uint16_t) (z * 1000.0);
		
	// Add the digital value to the Transmit buffer.
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_ACCELEROMETER;
	buffer->Data[(buffer->Count) + 2] = x_axis >> 8;
	buffer->Data[(buffer->Count) + 3] = x_axis >> 0;
	buffer->Data[(buffer->Count) + 4] = y_axis >> 8;
	buffer->Data[(buffer->Count) + 5] = y_axis >> 0;
	buffer->Data[(buffer->Count) + 6] = z_axis >> 8;
	buffer->Data[(buffer->Count) + 7] = z_axis >> 0;
	buffer->Count += LPP_ACCELEROMETER_SIZE;
}

void CayenneLPP::addBarometricPressure(uint8_t channel, float hpa) 
{
	int16_t val;
		
	// Check for invalid input pointer.
	if((buffer == 0) || ((buffer->Count + LPP_BAROMETRIC_PRESSURE_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
		
	val = (uint16_t)(hpa * 10.0);
		
	// Add the digital value to the Transmit buffer.
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_BAROMETRIC_PRESSURE;
	buffer->Data[(buffer->Count) + 2] = val >> 8;
	buffer->Data[(buffer->Count) + 3] = val >> 0;
	buffer->Count += LPP_BAROMETRIC_PRESSURE_SIZE;
}

void CayenneLPP::addGyrometer(uint8_t channel, float x, float y, float z) 
{
	
	int16_t vx, vy, vz;
			
	// Check for invalid input pointer or if adding this data will cause the message to be larger then the maximum payload size.
	if((buffer == 0) || ((buffer->Count + LPP_GYROMETER_SIZE) > LORAWAN_MAX_PAYLOAD))
	{
		return;
	}
		
	// Convert the floats to signed integers	
    vx = (int16_t)(x * 100.0);
    vy = (int16_t)(y * 100.0);
    vz = (int16_t)(z * 100.0);
			
	// Add the digital value to the Transmit buffer.
	buffer->Data[(buffer->Count) + 0] = channel;
	buffer->Data[(buffer->Count) + 1] = LPP_GYROMETER;
	buffer->Data[(buffer->Count) + 2] = vx >> 8;
	buffer->Data[(buffer->Count) + 3] = vx >> 0;
	buffer->Data[(buffer->Count) + 4] = vy >> 8;
	buffer->Data[(buffer->Count) + 5] = vy >> 0;
	buffer->Data[(buffer->Count) + 6] = vz >> 8;
	buffer->Data[(buffer->Count) + 7] = vz >> 0;
	buffer->Count += LPP_GYROMETER_SIZE;
}
