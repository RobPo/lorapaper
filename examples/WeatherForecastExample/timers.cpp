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
* File:     timers.cpp
* Author:   Adri Verhoef
* Company:	Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         06-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

/*
*****************************************************************************************
* INCLUDE FILES
*****************************************************************************************
*/
#include <stdint.h>
#include "avr/sleep.h"
#include "Arduino.h"
#include "timers.h"


void disable_ms_tick (void)
{
	TCCR1B = 0x00; // Disable Timer 1
}


/*
	@brief	Configures the timer to create an interrupt every millisecond for timing purposes.
*/
void enable_ms_tick (void)
{
	// Disable the Timer before configuring
	disable_ms_tick();
	
	// Use T1 since it's a 16 bit timer, but Timer 1 uses the same prescaler, so be aware.
	TCCR1A = 0x00;
	
	// Clear the timer counter.	
	TCNT1  = 0;	

	// (16MHz / 1) / 1000 = 16000 Ticks per millisecond.
	OCR1A = 16000; 
	
	// Enable Output compare A Match interrupts
	TIMSK1 = 0x02;
	
	// Enable Timer1 by setting a non-zero prescaler.
	TCCR1B = 0x09; // 0x00x0 1001: use CTC mode and set the prescaler to 1/1
}
