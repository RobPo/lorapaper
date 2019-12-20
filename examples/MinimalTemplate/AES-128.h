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
* File:     AES-128.h
* Author:   Gerben den Hartog
* Company:  Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
* Created on:         13-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/
#ifndef AES128_H
#define AES128_H

  #include <stdint.h>

	/**************************************************************************************
	* FUNCTION
	**************************************************************************************/

	void AES_Encrypt(uint8_t *Data, uint8_t *Key);
	void AES_Add_Round_Key(uint8_t *Round_Key, uint8_t (*State)[4]);
	uint8_t AES_Sub_Byte(uint8_t Byte);
	void AES_Shift_Rows(uint8_t (*State)[4]);
	void AES_Mix_Collums(uint8_t (*State)[4]);
	void AES_Calculate_Round_Key(uint8_t Round, uint8_t *Round_Key);
	void Send_State();

#endif
