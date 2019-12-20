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
* File:     LoRaMAC.h
* Author:   Gerben den Hartog
* Company:	Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
* Created on:         06-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#ifndef LORAMAC_H
#define LORAMAC_H

	/*********************************************************************************************
	* INCLUDES
	*********************************************************************************************/
	#include <stdint.h>
	#include "lorawan_def.h"

	/*********************************************************************************************
	* TYPE DEFINITION
	*********************************************************************************************/

	/******************************************************************************************
	* FUNCTION PROTOTYPES
	******************************************************************************************/
	class LORAMAC
	{
		public:
			LORAMAC(sLoRaWAN *LoRaPTR);
			~LORAMAC();

			void		init						(void);
			void		OTAA_connect				(void);
			void 		LORA_Send_Data				(void);
			void		LORA_send_and_receive		(void);
			void		LORA_increment_tx_channel	(void);
			RFM_RETVAL	LORA_Receive_Data			(eDR_CH *RxSettings, uint16_t Timeout_ms);
			void 		LoRa_Send_JoinReq			(void);
			void		Init_DevNonce_Generation	(void);
			void 		Generate_DevNonce			(uint8_t *DevNonce);
			
		private:
			sLoRaWAN	*lora;			// Pointer to the LoRaWAN structure with all the settings and information		
	};
#endif
