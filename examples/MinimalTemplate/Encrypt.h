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
* File:     Encrypt.h
* Author:   Gerben den Hartog
* Company:	Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
* Created on:         09-02-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#ifndef ENCRYPT_H
#define ENCRYPT_H

	/*
	********************************************************************************************
	* INCLUDES
	********************************************************************************************
	*/
	#include <stdint.h>
	#include "lorawan_def.h"

	/*
	*****************************************************************************************
	* FUNCTION PROTOTYPES
	*****************************************************************************************
	*/
	void 	Construct_Data_MIC	(uint8_t *data, uint8_t lenght, sLoRa_Session *Session_Data, sLoRa_Message *Message);
	void 	Calculate_MIC		(uint8_t *data, uint8_t lenght, uint8_t *Key, sLoRa_Message *Message);
	void 	Encrypt_Payload		(uint8_t *data, uint8_t lenght, uint8_t *Key, sLoRa_Message *Message);
	void 	Generate_Keys		(uint8_t *Key, 	uint8_t *K1, 	uint8_t *K2);
	void 	Shift_Left			(uint8_t *Data);
	void 	XOR					(uint8_t *New_Data,uint8_t *Old_Data);

#endif
