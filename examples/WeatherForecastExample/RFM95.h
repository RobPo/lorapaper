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
* File:     RFM95.h
* Author:   Gerben den Hartog
* Company: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         06-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#ifndef RFM95_H
#define RFM95_H

	/*********************************************************************************************
											INCLUDES
	*********************************************************************************************/
	#include <stdint.h>
	#include "lorawan_def.h"


	/*********************************************************************************************
											DEFINITIONS
	*********************************************************************************************/	
	#define RECEIVE_RX_TIMEOUT_MS 500
	

	/******************************************************************************************
											ENUMERATIONS
	******************************************************************************************/	
	typedef enum
	{
		MOTE_TRANSCEIVER,       // 	Default settings for the RFM to function as a mote which can send messages to and receive messages from a gateway.
		GATEWAY_TRANSMITTER,    //  IQ settings to let the RFM95 behave as an Gateway sending messages to a mote.
		GATEWAY_RECEIVER        //  IQ settings to let the RFM95 behave as an Gateway receiving messages from a mote.
	}IQ_FUNCTION;


	/******************************************************************************************
											FUNCTION PROTOTYPES
	******************************************************************************************/

	void 		RFM_Init				(sLoRaWAN *lora);
	void		RFM_Send_Package		(uint8_t *data, uint8_t lenght, eDR_CH *TxSettings, uint16_t TxPower, eCHANNEL_LIST *list);
	RFM_RETVAL	RFM_Single_Receive		(eDR_CH * RxSettings, uint16_t timeout_ms, eCHANNEL_LIST *list);
	void		RFM_Continuous_Receive	(eDR_CH * RxSettings, eCHANNEL_LIST *list);
	RFM_RETVAL 	RFM_Get_Package			(uint8_t *data, uint8_t *counter);
	void		RFM_Change_Datarate		(eLoRaWAN_DATARATES Datarate, uint16_t timeout_ms);
	void		RFM_Change_Channel		(eLoRaWAN_CHANNELS Channel, eCHANNEL_LIST *list);
	uint32_t	calculate_frequency_settings (uint32_t frequency_Hz);
	void 		RFM_Switch_Mode			(uint8_t Mode);
	void 		RFM_SetIQ				(IQ_FUNCTION function);
	void		RFM_Set_Output_Power	(uint8_t power);
	
	/* SPI functions */
	uint8_t 	RFM_Read				(uint8_t RFM_Address);
	void		RFM_Read_Array			(uint8_t RFM_Address,	uint8_t *RFM_Data, uint8_t lenght);
	void 		RFM_Write				(uint8_t RFM_Address, 	uint8_t RFM_Data);
	void		RFM_Write_Array			(uint8_t RFM_Address,	uint8_t *RFM_Data, uint8_t lenght);

#endif
