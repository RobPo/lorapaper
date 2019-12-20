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
* File:     DS2401.h
* Author:   Gerben den Hartog
* Company:	Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
* Created on:         13-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#ifndef DS2401_H
#define DS2401_H

	#include <stdint.h>
	#include <stdbool.h>
	
	/*********************************************************************************************
										DEFINITIONS
	*********************************************************************************************/
	#define RAM_SIZE 8

	/*********************************************************************************************
										FUNCTION PORTOTYPES
	*********************************************************************************************/
	bool	DS_Read		(uint8_t *DS_Bytes);
	bool	DS_CheckCRC	(uint8_t *DS_bytes);
	void 	DS_WR1		(void);
	void 	DS_WR0		(void);
	uint8_t DS_ReadByte	(void);
	
#endif
