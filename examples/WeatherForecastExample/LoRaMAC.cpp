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
* File:     LoRaMAC.cpp
* Author:   Gerben den Hartog
* Company:	Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
* Created on:         06-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/
/*
*****************************************************************************************
* INCLUDE FILES
*****************************************************************************************
*/
#include <EEPROM.h>
#include <stdint.h>
#include <stdio.h>
#include <SPI.h>
#include "spi_functions.h"
#include "AES-128.h"
#include "RFM95.h"
#include "Encrypt.h"
#include "LoRaMAC.h"
#include "timers.h"
#include "lorawan_def.h"
#include "lorapaper.h"

/******************************************************************************************
											FUNCTIONS
******************************************************************************************/
LORAMAC::LORAMAC(sLoRaWAN *LoRaPTR)
{
	// Check if the pointer is not invalid.
	if(LoRaPTR != NULL)
	{
		// Copy the LoRa structure to the pointer.
		lora = LoRaPTR;
	}
	else
	{
		//Re-Initialize the RFM95 module to set it to a lowe power state.
		RFM_Init(lora);
		
		lora = 0;
	}
}

LORAMAC::~LORAMAC()
{
	lora = NULL;
}


/******************************************************************************************
* Description :
* Arguments   :
******************************************************************************************/
void LORAMAC::init(void)
{
	// Check if the pointer is not invalid.
	if(lora == NULL)
	{
		return;
	}
				
	// Initialize the random number generation.
	Init_DevNonce_Generation();
		
	//Initialize the RFM95 module
	RFM_Init(lora);
  
}

/******************************************************************************************
* Description : 
* Arguments   : 
******************************************************************************************/
void LORAMAC::OTAA_connect(void)
{
	uint8_t attempt = 0;
	
	// Check for invalid lora pointer
	if(lora == NULL)
	{
		return;
	}
	
	// Check if the activation method is set to ABP. If so OTAA join procedure is not required.
	if(lora->activation_method == ACTIVATION_BY_PERSONALISATION)
	{
		return;
	}
	
	// Set the flag for OTAA done to false.
	lora->OTAA.OTAAdone = false;
	
	// Continue to retry the OTAA connection
	while(lora->OTAA.OTAAdone == false)
	{	
		// Wait ten seconds before attempting to connect with LoRaWAN
		/*
		delay(10000);
		Serial.print("Sending Join Request: ");
		Serial.println(attempt++, DEC);
    */
		// Transmit the join request to the back-end.
		LoRa_Send_JoinReq();
		
		// Enable the ms tick and clear the delay variable to time the JOIN_DELAY
		enable_ms_tick();
		lora->timeslot = 0;
			
		//	Wait for join delay 1
		while(lora->timeslot < JOIN_DELAY_1)
		{}
	
		// Receive the Join Accept from the Server with the Receive settings from the Transmit
		LORA_Receive_Data(&lora->OTAA.TxChDr, RECEIVE_RX_TIMEOUT_MS);
		
		// Check if the JOIN_ACCEPT message was received in time slot 1
		if(lora->OTAA.OTAAdone == true)
		{
			disable_ms_tick();
			return; // No need to wait for Time slot 2.
		}
		
		// Wait for join delay 2
		while(lora->timeslot < JOIN_DELAY_2)
		{}	
		
		// Configure the channel and datarate for Join delay 2 time slot.
		lora->OTAA.RxChDr.channel  = CH10_869_525;
		lora->OTAA.RxChDr.datarate = SF12_BW125kHz; 
		LORA_Receive_Data(&lora->OTAA.RxChDr, RECEIVE_RX_TIMEOUT_MS);
		
		disable_ms_tick();
		
		// Check if the JOIN_ACCEPT message was received in time slot 2
		if(lora->OTAA.OTAAdone == true)
		{
			return; // No need to wait for Time slot 2.
		}
		else
		{
			// Increment the OTAA channel for the next join attempt
			switch(lora->OTAA.TxChDr.channel)
			{
				case CH00_868_100:
					lora->OTAA.TxChDr.channel = CH01_868_300;
					break;
				case CH01_868_300:
					lora->OTAA.TxChDr.channel = CH02_868_500;
					break;
				case CH02_868_500:
				default:
					lora->OTAA.TxChDr.channel = CH00_868_100;
					break;
			}
		}
	}	
}


/******************************************************************************************
* Description : Function for transmitting data Confirmed Up and then receive the back-ends
*				reply.
* Arguments   : lora pointer to the complete LoRaWAN structure.
******************************************************************************************/
void LORAMAC::LORA_send_and_receive (void)
{
	// Check for invalid pointer
	if(lora == NULL)
	{
		return;
	}
	
	// Set the TX message to confirmed up, so the back-end must reply. Copy the TX settings as the LORA_Send_Data will increment the Tx channel when channel hopping is enabled.
	lora->TX.Confirmation = CONFIRMED;
	lora->Session.RxChDr.channel  = lora->Session.TxChDr.channel;
	lora->Session.RxChDr.datarate = lora->Session.TxChDr.datarate;
	
	// Transmit data in the TX buffer.
	LORA_Send_Data();
	
	// Enable the ms tick and clear the delay variable to time the RECEIVE_DELAY1
	enable_ms_tick();
	lora->timeslot = 0;
	
	//	Wait for time slot 1
	while(lora->timeslot < RECEIVE_DELAY1)
	{}
		
	// Receive the back-end reply from the gateway with the settings from the Transmission.
	if(LORA_Receive_Data(&(lora->Session.RxChDr), RECEIVE_RX_TIMEOUT_MS) == RX_MESSAGE)
	{
		// On success, disable the ms tick timer and return with the data.
		disable_ms_tick();
		return; // No need to wait for Receive slot 2.
	}
	
	//	Wait for time slot 2 before 
	while(lora->timeslot < RECEIVE_DELAY2)
	{}	

	// Configure the channel and datarate for Join delay 2 time slot.
	lora->Session.RxChDr.channel  = CH10_869_525;
	lora->Session.RxChDr.datarate = SF09_BW125kHz;
	
	// Disable the timer.
	disable_ms_tick();
		
	// Receive the reply from the Server with the Receive settings from the Transmit
	LORA_Receive_Data(&(lora->Session.RxChDr), RECEIVE_DELAY2);
}


/*
*****************************************************************************************
* Description : Function that is used to build a LoRaWAN data message and then transmit it.
*
* Arguments   : *Data_Tx pointer to tranmit buffer
*				*Session_Data pointer to sLoRa_Session structure
*				*LoRa_Settings pointer to sSetting struct
*****************************************************************************************
*/
void LORAMAC::LORA_Send_Data(void)
{
	uint8_t RFM_Data[LORA_FIFO_SIZE], RFM_Counter;
	uint8_t i;
	
	// Check for invalid lora pointer
	if(lora == NULL)
	{
		return;
	}
	
	// Check whether the data payload is within the array size.
	if(lora->TX.Count > LORA_FIFO_SIZE)
	{
		lora->TX.Count = LORA_FIFO_SIZE;
	}

	lora->TX.Frame_Port    = 1;		//Frame port always 1 for now
	lora->TX.Frame_Control = 0;

	//Load device address from session data into the message
	lora->TX.DevAddr[0] = lora->Session.DevAddr[0];
	lora->TX.DevAddr[1] = lora->Session.DevAddr[1];
	lora->TX.DevAddr[2] = lora->Session.DevAddr[2];
	lora->TX.DevAddr[3] = lora->Session.DevAddr[3];

	//Load the frame counter from the session data into the message
	lora->TX.Frame_Counter = lora->Session.frame_counter_up;

	//Set confirmed or unconfirmed bit 
	if(lora->TX.Confirmation == CONFIRMED)
	{
		lora->TX.MAC_Header = CONFIRMED_DATA_UP; // 0x80
	}
	//Confirmed
	else
	{
		lora->TX.MAC_Header = UNCONFIRMED_DATA_UP; // 0x40
	}

	/* Build the LoRaWAN Package */

	// Load mac header
	RFM_Data[0] = (uint8_t)lora->TX.MAC_Header;

	//Load the device address in reverse order
	RFM_Data[1] = lora->TX.DevAddr[3];
	RFM_Data[2] = lora->TX.DevAddr[2];
	RFM_Data[3] = lora->TX.DevAddr[1];
	RFM_Data[4] = lora->TX.DevAddr[0];

	// Load the frame control
	RFM_Data[5] = lora->TX.Frame_Control;

	//Load frame counter
	RFM_Data[6] = (uint8_t)(lora->Session.frame_counter_up >> 0);
	RFM_Data[7] = (uint8_t)(lora->Session.frame_counter_up >> 8);

	//Set data counter to 8
	RFM_Counter = 8;
	
	//If there is data load the Frame_Port field, encrypt the data and load the data
	if(lora->TX.Count != 0)
	{
		//Load Frame port field
		RFM_Data[8] = lora->TX.Frame_Port;

		//Raise package counter for the Frame Port field
		RFM_Counter++;

		// Copy the data into the RFM buffer.
		memcpy(&(RFM_Data[RFM_Counter]), &(lora->TX.Data[0]), lora->TX.Count);
		
		// Encrypt the copied data in the RFM_Data array, so the original contents of the TX.Data array are not overwritten by the encryption.
		Encrypt_Payload(&(RFM_Data[RFM_Counter]), lora->TX.Count, &(lora->Session.AppSKey[0]), &(lora->TX));
		
		//Add data Length to package counter
		RFM_Counter += lora->TX.Count;
	}
	
	//Calculate MIC
	Construct_Data_MIC(&(RFM_Data[0]), RFM_Counter, &(lora->Session), &(lora->TX));
	
	// Load the calculated MIC in the RFM transmit array.
	memcpy(&(RFM_Data[RFM_Counter]), &(lora->TX.MIC[0]), 4);

	//Add MIC length to RFM package length
	RFM_Counter += 4;

	// Send Package to the RFM and transmit the data.
	RFM_Send_Package(RFM_Data, RFM_Counter, &(lora->Session.TxChDr), lora->Session.Transmit_Power, &(lora->CH_list));

	// Raise Frame counter
	if(lora->Session.frame_counter_up != UINT16_MAX)
	{
		//Raise frame counter
		lora->Session.frame_counter_up++;
	}
	else
	{
		// End of session is reached. 
		if(lora->activation_method == ACTIVATION_BY_PERSONALISATION)
		{
			// Clear the frame counter when using ABP	
			lora->Session.frame_counter_up = 0;
		}
		else
		{
			// When using OTAA begin a new session to reset the frame counter.
			
		}
	}

	//Change channel for next message if hopping is activated
	LORA_increment_tx_channel();
}


/*
*****************************************************************************************
* Description : Function that is used to receive a LoRaWAN message and retrieve the data from the RFM
*               Also checks on CRC, MIC and Device Address
*               This function is used for Class A and C motes.
*
* Arguments   : *Data_Rx pointer to receive buffer
*				*Session_Data pointer to sLoRa_Session struct
*				*OTAA_Data pointer to sLoRa_OTAA struct
*				*Message_Rx pointer to sLoRa_Message struct used for the received message information
*				*LoRa_Settings pointer to sSetting struct
*****************************************************************************************
*/
RFM_RETVAL LORAMAC::LORA_Receive_Data(eDR_CH *RxSettings, uint16_t Timeout_ms)
{
	uint8_t RFM_Data[LORA_FIFO_SIZE], RFM_Counter;
	uint8_t i, N;
	uint8_t Frame_Options_Length;
	uint8_t Data_Location;
	uint32_t frequency;
	
	// Check for invalid lora pointer
	if(lora == NULL)
	{
		return LORA_POINTER_INVALID;
	}
	

	// If it is a type A device switch RFM to single receive
	if(lora->Mote_Class == CLASS_A)
	{
		lora->RX.retVal = RFM_Single_Receive(RxSettings, Timeout_ms, &(lora->CH_list)); 
	}
	else
	{
		//Switch RFM to standby
		RFM_Switch_Mode(0x01);
		lora->RX.retVal = NEW_MESSAGE;
	}

	// If there is a message received get the data from the RFM
	if(lora->RX.retVal == NEW_MESSAGE)
	{
		lora->RX.retVal = RFM_Get_Package(&RFM_Data[0], &RFM_Counter);

		//If mote class C switch RFM back to continuous receive
		if(lora->Mote_Class == CLASS_C)
		{
			//Switch RFM to Continuous Receive
			RFM_Continuous_Receive(RxSettings, &(lora->CH_list));
		}
	}

	/* Clear the DIO# pins of the RFM, then switch the RFM to sleep */
	SPI_Write(RFM_NSS, 0x12, 0xE0);
	SPI_Write(RFM_NSS, 0x01, 0x80);

	// If CRC is not OK, return with CRC_NOK to indicate that the received messages is incomplete.
	if(lora->RX.retVal != CRC_OK)
	{
		return CRC_NOK;
	}
	
	// Get the MAC_Header
	lora->RX.MAC_Header = (eMESSAGE_TYPES) RFM_Data[0];

	//Join Accept message
	switch(lora->RX.MAC_Header) // 0x20
	{
		///------------------------------------------------------------------------------------------------------------------------------------------------------
		case JOIN_ACCEPT:

			// Decrypt the data
			for(i = 0 ; i < ((RFM_Counter - 1) >> 4) ; i++)
			{
				AES_Encrypt(&(RFM_Data[(i << 4)+1]), lora->OTAA.AppKey);
			}

			// Calculate MIC
			RFM_Counter -= 4;

			// Get MIC
			Calculate_MIC(RFM_Data, RFM_Counter, lora->OTAA.AppKey, &(lora->RX));

			// Check if the calculated and received MIC match or not. 
			if(memcmp(&(RFM_Data[RFM_Counter]), &(lora->RX.MIC), 4) == 0)
			{
				lora->RX.retVal = MIC_OK;
			}
			else
			{
				// When the MIC is NOK, return the MIC_NOK_OTAA
				return MIC_NOK_OTAA;
			}

			// Get AppNonce
			lora->OTAA.AppNonce[0] = RFM_Data[1];
			lora->OTAA.AppNonce[1] = RFM_Data[2];
			lora->OTAA.AppNonce[2] = RFM_Data[3];

			// Get Net ID
			lora->OTAA.NetID[0] = RFM_Data[4];
			lora->OTAA.NetID[1] = RFM_Data[5];
			lora->OTAA.NetID[2] = RFM_Data[6];

			// Get session Device address
			lora->Session.DevAddr[3] = RFM_Data[7];
			lora->Session.DevAddr[2] = RFM_Data[8];
			lora->Session.DevAddr[1] = RFM_Data[9];
			lora->Session.DevAddr[0] = RFM_Data[10];
			
			// Get the DLsettings
			lora->CH_list.rx1_dr_offset = (RFM_Data[11] & 0x70) >> 4;
			lora->CH_list.rx2_dr = RFM_Data[11] & 0x0F;
			
			// Get the RXDelay
			lora->CH_list.rx_delay = RFM_Data[12];

			// Calculate Network Session Key
			lora->Session.NwkSKey[0] = 0x01;

			// Load AppNonce
			lora->Session.NwkSKey[1] = lora->OTAA.AppNonce[0];
			lora->Session.NwkSKey[2] = lora->OTAA.AppNonce[1];
			lora->Session.NwkSKey[3] = lora->OTAA.AppNonce[2];

			// Load NetID
			lora->Session.NwkSKey[4] = lora->OTAA.NetID[0];
			lora->Session.NwkSKey[5] = lora->OTAA.NetID[1];
			lora->Session.NwkSKey[6] = lora->OTAA.NetID[2];

			// Load Dev Nonce
			lora->Session.NwkSKey[7] = lora->OTAA.DevNonce[0];
			lora->Session.NwkSKey[8] = lora->OTAA.DevNonce[1];

			// Pad with zeros
			lora->Session.NwkSKey[9]  = 0x00;
			lora->Session.NwkSKey[10] = 0x00;
			lora->Session.NwkSKey[11] = 0x00;
			lora->Session.NwkSKey[12] = 0x00;
			lora->Session.NwkSKey[13] = 0x00;
			lora->Session.NwkSKey[14] = 0x00;
			lora->Session.NwkSKey[15] = 0x00;

			// Copy to AppSkey
			memcpy(lora->Session.AppSKey, lora->Session.NwkSKey, 16);

			//Change first byte of AppSKey
			lora->Session.AppSKey[0] = 0x02;

			// Calculate the keys
			AES_Encrypt(lora->Session.NwkSKey, lora->OTAA.AppKey);
			AES_Encrypt(lora->Session.AppSKey, lora->OTAA.AppKey);

			// Reset Frame counters
			lora->Session.frame_counter_up   = 0;
			lora->Session.frame_counter_down = 0;
			
			// Check for the optional CF list in the JOin Accept message
			if(RFM_Counter > 13)
			{
				//printStringAndHex("Join Accept Message: ", RFM_Data,  RFM_Counter);
				// Calculate the number of frequencies 
				N = (RFM_Counter - 14) / 3;
				
				// Maximum of 5 frequencies
				if(N > CFLIST_FREQUENCIES_MAX)
				{
					N = CFLIST_FREQUENCIES_MAX;
				}
				
				//Serial.println("CFlist");
				
				// Retrieve all frequencies and print them to the serial port.
				for( i = 0 ; i < N ; i++)
				{
					// Construct the frequency value and multiply the value with 100 to get the frequency in Hertz.
					frequency = (((uint32_t)(RFM_Data[13 + (i*3)]) <<  0) | ((uint32_t)(RFM_Data[14 + (i*3)]) <<  8) | ((uint32_t)(RFM_Data[15 + (i*3)]) << 16)) * 100;
					
					
					// Check if the frequency is between the 867.0MHz and 868.0 MHz and the boundaries of the channel list haven't been reached yet.
					if((frequency > 867000000) && (frequency < 868000000) && (lora->CH_list.index <= CFLIST_FREQUENCIES_MAX))
					{
						// Add the retrieved frequency to the list of channel settings and increment the index;
						lora->CH_list.channel[(lora->CH_list.index)] = calculate_frequency_settings(frequency);
						lora->CH_list.index += 1;
					}
				} 	
			}
			
			// Print the received variables
			/*
			printStringAndHex("Device Address: ",			lora->Session.DevAddr, 4);
			printStringAndHex("Network Session Key: ",		lora->Session.NwkSKey, 16);
			printStringAndHex("Application Session Key: ",	lora->Session.AppSKey, 16);
*/
			//Clear Data counter
			lora->RX.Count = 0;
			lora->OTAA.OTAAdone = true;
			lora->RX.retVal = OTAA_COMPLETE;
			break;
		
		///------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		case UNCONFIRMED_DATA_UP:
		case UNCONFIRMED_DATA_DOWN:
		case CONFIRMED_DATA_UP:
		case CONFIRMED_DATA_DOWN:

			//Get device address from received data
			lora->RX.DevAddr[0] = RFM_Data[4];
			lora->RX.DevAddr[1] = RFM_Data[3];
			lora->RX.DevAddr[2] = RFM_Data[2];
			lora->RX.DevAddr[3] = RFM_Data[1];

			//Get frame control field
			lora->RX.Frame_Control = RFM_Data[5];

			 //Get the frame counter by combining two bytes
			lora->RX.Frame_Counter = ((uint16_t)RFM_Data[7] << 8) | (uint16_t)RFM_Data[6];

			//Lower Package length with 4 to remove MIC length
			RFM_Counter -= 4;

			//Calculate MIC
			Construct_Data_MIC(&RFM_Data[0], RFM_Counter, &(lora->Session), &(lora->RX));
							
			// Compare the calculated MIC and the Received MIC.
			if(memcmp(&(RFM_Data[RFM_Counter]), &(lora->RX.MIC[0]), 4) != 0)
			{
				lora->RX.retVal = MIC_NOK_MESSAGE;
				return MIC_NOK_MESSAGE;
			}

			// Compare the Received message's device address and the session device address to check whether this message was intended for this mote.
			if(memcmp(lora->RX.DevAddr, lora->Session.DevAddr, 4) != 0)
			{
				//Return when ADDRESS_NOK
				lora->RX.retVal = ADDRESS_NOK;
				return ADDRESS_NOK;
			}

			//Get length of frame options field
			Frame_Options_Length = (lora->RX.Frame_Control & 0x0F);

			//Add length of frame options field to data location
			Data_Location = 8 + Frame_Options_Length;

			//Check if there's is data in the package of not. OTherwise there might be 
			if(RFM_Counter == Data_Location)
			{
				// The Received number of bytes and the data start location match, so there's no data in the received message.
				RFM_Counter = 0;
			}
			else
			{
				//Get port field when there is data
				lora->RX.Frame_Port = RFM_Data[8];

				//Calculate the amount of data in the package
				lora->RX.Count = (RFM_Counter - Data_Location - 1);

				//Correct the data location by 1 for the Fport field
				Data_Location++;

				// Copy and decrypt the data
				memcpy(lora->RX.Data, &(RFM_Data[Data_Location]), lora->RX.Count);

				if(lora->RX.Frame_Port == 0)
				{
					Encrypt_Payload(lora->RX.Data, lora->RX.Count, lora->Session.NwkSKey, &(lora->RX));
				}
				else
				{
					Encrypt_Payload(lora->RX.Data, lora->RX.Count, lora->Session.AppSKey, &(lora->RX));
				}				
			}
			lora->RX.retVal = RX_MESSAGE;	
			break;
		///-------------------------------------------------------------------------------------------------------------------------------------------------------
		default:
			lora->RX.retVal = MAC_HEADER_NOK;
			break;
	}
	// Return the message status 
	return lora->RX.retVal;
}

/******************************************************************************************
* Description : Function that is used to generate device nonce used in the join request function
*				This is based on a pseudo random function in the Arduino library
*
* Arguments   : *Devnonce pointer to the devnonce arry of withc is uint8_t[2]
******************************************************************************************/
void LORAMAC::Generate_DevNonce(uint8_t *DevNonce)
{
	uint16_t RandNumber;
	
	// Check for invalid lora pointer
	if(DevNonce == NULL)
	{
		return;
	}
	// Generate a random number between 0x0000 to 0xFFFF	
	RandNumber = random(0xFFFF);
	//Serial.println(RandNumber, DEC);

	// Return the Dev Nonce value.
	DevNonce[0] = (uint8_t)(RandNumber >> 0);
	DevNonce[1] = (uint8_t)(RandNumber >> 8);
}

/*
*****************************************************************************************
* Description : Function that is used to send a join request to a network.
*
* Arguments   : *OTAA_Data pointer to sLoRa_OTAA struct
*				*LoRa_Settings pointer to sSetting struct
*****************************************************************************************
*/
void LORAMAC::LoRa_Send_JoinReq(void)
{
    uint8_t i;
	
	// Check for invalid lora pointer
	if(lora == NULL)
	{
		return;
	}

	//Initialize RFM data buffer
    uint8_t data[23], lenght;

	// Set the OTAA done to false, so the higher layer code can detect if OTAA has been completed successfully
	lora->OTAA.OTAAdone = false;
    lora->TX.MAC_Header = JOIN_REQUEST; //Join request 0x00

    //Construct OTAA Request message
    //Load Header in package
    data[0] = lora->TX.MAC_Header;

    //Load AppEUI in package
    for(i = 0 ; i < 8 ; i++)
    {
        data[i+1] = lora->OTAA.AppEUI[7-i];
    }

    //Load DevEUI in package
    for(i = 0; i < 8; i++)
    {
        data[i+9] = lora->OTAA.DevEUI[7-i];
    }

    //Generate DevNonce
    Generate_DevNonce(&(lora->OTAA.DevNonce[0]));

    //Load DevNonce in package
    data[17] = lora->OTAA.DevNonce[0];
    data[18] = lora->OTAA.DevNonce[1];

    //Set length of package
    lenght = 19;

    //Get MIC
    Calculate_MIC(data, lenght, lora->OTAA.AppKey, &(lora->TX));

    //Load MIC in package
	memcpy(&(data[19]), &(lora->TX.MIC[0]), 4);

    //Set lenght of package to the right length
    lenght = 23;

    //Send Package
    RFM_Send_Package(data, lenght, &(lora->OTAA.TxChDr), lora->OTAA.Transmit_Power, &(lora->CH_list));
}



/********************************************************************************************************************************************************
* Description : Initialize the random number generation seed, so all generated random numbers are new when generated.
********************************************************************************************************************************************************/
#define EEADDR 0x3FF
void LORAMAC::Init_DevNonce_Generation(void)
{
	uint8_t RandSeed;
	unsigned int addr = 0;
	
	RandSeed = eeprom_read_byte((uint8_t*)addr);
	//Serial.print("Random Seed: ");
	//Serial.println(RandSeed, HEX);
	
	eeprom_write_byte((uint8_t*)addr, (RandSeed+1));
	
	// Initialize the Random seed with the retrieved seed value
	randomSeed(RandSeed);
}


/********************************************************************************************************************************************************
* Description : Increment the transmit channel according to the connection methode and the available CF list from the Over The Air Activation.
********************************************************************************************************************************************************/
void LORAMAC::LORA_increment_tx_channel (void)
{
	// Check for invalid pointer
	if(lora == NULL)
	{
		return;
	}
	
	// Check whether channel hopping is enabled or not.
	if(lora->CH_list.channel_hopping_on == false)
	{
		return;
	}
	
	// Increment the Transmit channel
	lora->Session.TxChDr.channel = (eLoRaWAN_CHANNELS)(lora->Session.TxChDr.channel + 1);
	
	// Depending on the activation method, increment the transmission channel to a maximum channel  
	if(lora->activation_method == ACTIVATION_BY_PERSONALISATION)
	{
		// With ABP only three channels are available 
		if(lora->Session.TxChDr.channel > CH02_868_500)
		{
			lora->Session.TxChDr.channel = CH00_868_100;
		}
	}
	else
	{
		// Check if the incremented channel is above the number of received channels from OTAA
		if(lora->Session.TxChDr.channel > ((eLoRaWAN_CHANNELS) CH02_868_500 + (lora->CH_list.index)))
		{
			lora->Session.TxChDr.channel = CH00_868_100;
		}
	}
}
