/******************************************************************************************
* Copyright 2017 Ideetron B.V.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************************/
/****************************************************************************************
* File:     mcp7940.h
* Author:   Adri Verhoef
* Company:	Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
* Created on:         22-09-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/
#include "HardwareSerial.h"
#include "I2C.h"
#include "mcp7940.h"

// private functions 
uint8_t convert_ASCII_to_decimal	(uint8_t *ASCII);
uint8_t convert_binary_to_decimal	(uint8_t mask, uint8_t regVal);
uint8_t convert_decimal_to_binary	(uint8_t mask, uint8_t decimal);



/*
	@brief	
		Initialize the mcp7940 Real Time Clock (RTC) with the compilation strings to set the current date and time. Also configures the RTC to generate an 
		interrupt on the next minute roll-over with the alarm 0 function of the RTC. The RTC will pull the MFP pin low, which is held high with an internal
		pull-up on pin D3.
	@parameters	
		*TimeDate			Pointer to a structure where the current time will be written to once converted from the compiler __BUILD__ and __TIME__ strings.
		alarm_in_x_minutes	The number of minutes from the current time on which the RTC will generate an interrupt.		
*/
void mcp7940_init (sTimeDate *TimeDate, uint8_t alarm_in_x_minutes)
{
	uint8_t date [] = __DATE__; // Sep 27 2017
	uint8_t time [] = __TIME__; // 13:50:25
	
	//	Enable the internal pull-up on pin D3 by setting D3 bit in PORTD and DDRD as the MFP of the RTC is open drain and using an internal pull-up is more efficient.
	PORTD |= 0x08;
	DDRD &= ~0x08;
	
	//Serial.println((char*)date);
	//Serial.println((char*)time);
	
	// Convert the compilation time and date to decimal numbers in order to set the RTC time.
	TimeDate->hours		= convert_ASCII_to_decimal(&(time[0]));
	TimeDate->minutes	= convert_ASCII_to_decimal(&(time[3]));
	TimeDate->seconds	= convert_ASCII_to_decimal(&(time[6]));
	TimeDate->day		= convert_ASCII_to_decimal(&(date[4]));
	TimeDate->year		= convert_ASCII_to_decimal(&(date[9]));
	TimeDate->weekDay	= 1;	// There's no weekday in the time and date compiler string, so always set it to 1 upon start.
	
	// Compare the string of three letters to determine the current month as a decimal value
	if(memcmp(date, "Jan", 3) == 0)
	{
		TimeDate->month = 1;
	}else if (memcmp(date, "Feb", 3) == 0)
	{
		TimeDate->month = 2;
	}else if (memcmp(date, "Mar", 3) == 0)
	{
		TimeDate->month = 3;
	}else if (memcmp(date, "Apr", 3) == 0)
	{
		TimeDate->month = 4;
	}else if (memcmp(date, "May", 3) == 0)
	{
		TimeDate->month = 5;
	}else if (memcmp(date, "Jun", 3) == 0)
	{
		TimeDate->month = 6;
	}else if (memcmp(date, "Jul", 3) == 0)
	{
		TimeDate->month = 7;
	}else if (memcmp(date, "Aug", 3) == 0)
	{
		TimeDate->month = 8;
	}else if (memcmp(date, "Sep", 3) == 0)
	{
		TimeDate->month = 9;
	}else if (memcmp(date, "Oct", 3) == 0)
	{
		TimeDate->month = 10;
	}else if (memcmp(date, "Nov", 3) == 0)
	{
		TimeDate->month = 11;
	}else if (memcmp(date, "Dec", 3) == 0)
	{
		TimeDate->month = 12;
	}
	//Serial.print("Converted: ");
	//mcp7940_print(TimeDate);
	
	// Write the converted time and date settings to the RTC and print it to the serial port.
	mcp7940_set_time_and_date(TimeDate);
	mcp7940_print(TimeDate);
	
	I2C_write_register(MCP7940_SLAVE_ADDRESS, CONTROL, 0x00);	// Disable the square wave output on the RTC and Alarm 0
	
	// Reload alarm 0 to generate an interrupt over x minutes
	if(alarm_in_x_minutes > 0)
	{
		mcp7940_reset_minute_alarm(alarm_in_x_minutes);
	}
	
	// Enable Alarm 0, set the MFP pin to idle HIGH, active LOW.
	I2C_write_register(MCP7940_SLAVE_ADDRESS, CONTROL, 0x10);
		
	// read back the set time and date in order to check if the time and date was set properly.
	mcp7940_read_time_and_date(TimeDate);
		
	// Enable the Alarm to generate an external interrupt on a falling edge of INT1
	EICRA = 0x08;	//	The falling edge of INT1 generates an interrupt request.
	EIMSK = 0x02;	//	Enable the external interrupt
}


/*
	@brief	
		Function to disable the RTC and all alarms.	
*/
void mcp7940_disable (void)
{
	//	Disable all alarms and square wave outputs.
	I2C_write_register(MCP7940_SLAVE_ADDRESS, CONTROL, 0x00);
	
	//	Disable the RTC by clearing bit 7 of the RTC_SEC register, which will stop the 32kHz crystal.
	I2C_write_register(MCP7940_SLAVE_ADDRESS, RTC_SEC, 0x00);
}
	
	



/*
	@brief
		After the RTC's alarm has triggered, the alarm must be re-configured to generate an alarm again after  alarm_in_x_minutes minutes.
	@parameters
		alarm_in_x_minutes	The number of minutes from now when the RTC must generate an interrupt again.
*/
void mcp7940_reset_minute_alarm (uint8_t alarm_in_x_minutes)
{
	uint8_t value, current_time_minutes, new_time_minutes;
	
	if(alarm_in_x_minutes > 59)
	{
		alarm_in_x_minutes = 59;
	}
		
	// Read the current minutes register to calculate the match value for the minute register.
	current_time_minutes = convert_binary_to_decimal(MIN_MASK,  I2C_read_register(MCP7940_SLAVE_ADDRESS, RTC_MIN));
	
	// Calculate the new time settings.
	new_time_minutes = current_time_minutes + alarm_in_x_minutes;
		
	//	Check whether the additional number of minutes and the current time settings will be larger than 59. if larger, subtract 59 from the sum to calculate the new time
	if(new_time_minutes > 59)
	{
		// Subtract 59 from the sum of the current time in minutes and the added number of minutes.
		new_time_minutes -= 59;
	}	

	// Rewrite the Alarm 0 minutes register.
	I2C_write_register(MCP7940_SLAVE_ADDRESS, ALARM0_MIN, convert_decimal_to_binary(MIN_MASK, new_time_minutes));	
	
	/* 
		Set MFP pin High on Alarm interrupt on minutes match and clear the ALM0IF (Alarm interrupt Flag) now that the Alarm 0 minutes register is different
		from the current time or the interrupt would re-trigger. Read the register first to keep the current weekday value and add the three bits 2:0 to 
		the value written to the register.
	*/
	value = I2C_read_register(MCP7940_SLAVE_ADDRESS, ALARM0_WEEKDAY);
	I2C_write_register(MCP7940_SLAVE_ADDRESS, ALARM0_WEEKDAY, 0x10 | (value & 0x07));
}



/*
	@brief
		Function to read the time and date from the RTC, convert it to decimal values and store the values in the reference structure.
	@parameters
		TimeDate	pointer to the structure where all the retrieved time and date must be stored.
*/
void mcp7940_read_time_and_date (sTimeDate *TimeDate)
{
	uint8_t regVal;
	
	// Check the given pointer
	if(TimeDate == 0)
		return;
	
	// Read the Time and date from the RTC and convert it to a time and date in decimal values.
	TimeDate->seconds	= convert_binary_to_decimal(SEC_MASK,  I2C_read_register(MCP7940_SLAVE_ADDRESS, RTC_SEC));
	TimeDate->minutes	= convert_binary_to_decimal(MIN_MASK,  I2C_read_register(MCP7940_SLAVE_ADDRESS, RTC_MIN));
	TimeDate->hours		= convert_binary_to_decimal(HOUR_MASK, I2C_read_register(MCP7940_SLAVE_ADDRESS, RTC_HOUR));
	TimeDate->weekDay	=                       WEEKDAY_MASK & I2C_read_register(MCP7940_SLAVE_ADDRESS, RTC_WEEKDAY);
	
	TimeDate->day		= convert_binary_to_decimal(DATE_MASK,  I2C_read_register(MCP7940_SLAVE_ADDRESS, RTC_DATE));
	TimeDate->month		= convert_binary_to_decimal(MONTH_MASK, I2C_read_register(MCP7940_SLAVE_ADDRESS, RTC_MONTH));
	TimeDate->year		= convert_binary_to_decimal(YEAR_MASK,  I2C_read_register(MCP7940_SLAVE_ADDRESS, RTC_YEAR));
}


/*
	@brief
		Function to write the referenced time and date in the TimeDate structure to the RTC
	@parameters
		TimeDate	pointer to the structure where the new time and date settings are stored.
*/
void mcp7940_set_time_and_date (sTimeDate *TimeDate)
{
	uint8_t regVal;
	
	// Check the given pointer
	if(TimeDate == 0)
	{
		return;
	}
		
	//	Disable the oscillator to stop the RTC while the time and date are configured to prevent roll-overs from occurring while writing new values.
	I2C_write_register(MCP7940_SLAVE_ADDRESS, RTC_SEC,		0x00);	
	I2C_write_register(MCP7940_SLAVE_ADDRESS, RTC_MIN,		convert_decimal_to_binary(MIN_MASK,		TimeDate->minutes));
	I2C_write_register(MCP7940_SLAVE_ADDRESS, RTC_HOUR,		0x40 | convert_decimal_to_binary(HOUR_MASK,	TimeDate->hours));
	I2C_write_register(MCP7940_SLAVE_ADDRESS, WEEKDAY_MASK, convert_decimal_to_binary(WEEKDAY_MASK, TimeDate->weekDay));
	
	I2C_write_register(MCP7940_SLAVE_ADDRESS, RTC_DATE,		convert_decimal_to_binary(DATE_MASK,  TimeDate->day));
	I2C_write_register(MCP7940_SLAVE_ADDRESS, RTC_MONTH,	convert_decimal_to_binary(MONTH_MASK, TimeDate->month));
	I2C_write_register(MCP7940_SLAVE_ADDRESS, RTC_YEAR,		convert_decimal_to_binary(YEAR_MASK,  TimeDate->year));
	
	// Write the seconds as last since, we'll reactivate the oscillator with this write operation as well.
	I2C_write_register(MCP7940_SLAVE_ADDRESS, RTC_SEC,  0x80 | convert_decimal_to_binary(SEC_MASK,  TimeDate->seconds));
}



/*
	@brief
		Convert the RTC's tens and ones binary format to a decimal number.
	@parameters
		mask	Mask value for bits 7:4, since the time and date registers of the RTC do not use all bits or additional functionalities are included in the bits
				for the tens values, which should not be converted to decimal values.
		regVal	New binary value which needs to be converted to decimal
*/
uint8_t convert_binary_to_decimal (uint8_t mask, uint8_t regVal)
{
	return (((regVal & mask) >> 4) * 10) + (regVal & 0x0F);
}


/*
	@brief
		Convert a decimal value to binary tens and ones values.
	@parameters
		mask	Mask value for bits 7:4, since the time and date registers of the RTC do not use all bits or additional functionalities are included in the bits
				for the tens values, which should not be converted to decimal values.
		regVal	New decimal value which needs to be converted to binary
*/
uint8_t convert_decimal_to_binary (uint8_t mask, uint8_t decimal)
{
	uint8_t tens, ones;
	tens = decimal / 10;
	ones = decimal % 10;
	return ((mask &(tens << 4)) | (ones & 0x0F));
}

/*
	@brief
		Converts two ASCII characters to a single decimal value to convert the compilers __BUILD__ and __TIME__ string to decimal values.
	@parameters
		ASCII	pointer to the first ASCII character.
*/
uint8_t convert_ASCII_to_decimal (uint8_t *ASCII)
{
	uint8_t retVal = 0;
	
	if((ASCII[0] >= '0') && (ASCII[0] <= '9'))
	{
		retVal = (ASCII[0] - '0') * 10;
	}
	
	if((ASCII[1] >= '0') && (ASCII[1] <= '9'))
	{
		retVal += (ASCII[1] - '0');
	}
	
	return retVal;
}

/*
	@brief	
		Prints the referenced structure in a human readable format to the serial port "hh:mm:ss day:n	dd/mm/yy"
*/
void mcp7940_print(sTimeDate *TimeDate)
{
	Serial.print(TimeDate->hours, DEC);
	Serial.print(':');
	Serial.print(TimeDate->minutes, DEC);
	Serial.print(':');
	Serial.print(TimeDate->seconds, DEC);
	Serial.print(" day:");
	Serial.print(TimeDate->weekDay, DEC);
	
	Serial.print('\t');
	
	Serial.print(TimeDate->day, DEC);
	Serial.print('/');
	Serial.print(TimeDate->month, DEC);
	Serial.print('/');
	Serial.println(TimeDate->year, DEC);
}
