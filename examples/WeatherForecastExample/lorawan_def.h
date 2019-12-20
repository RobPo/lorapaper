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
* File:     Struct.h
* Author:   Gerben den Hartog
* Company:	Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
* Created on:         06-02-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/


#ifndef STRUCT_H
#define STRUCT_H

	#include <stdint.h>
	#include <stdbool.h>

	#define LORA_FIFO_SIZE			64
	#define CFLIST_FREQUENCIES_MAX	5
	#define DEFAULT_FREQUENCIES		3
	#define LORAWAN_MAX_PAYLOAD		52
	
	/*********************************************************************************************
										ENUMERATION DEFENITIONS
	*********************************************************************************************/
	// LoRaWAN Mote class specification
	typedef enum
	{
		CLASS_A,
		//CLASS_B,
		CLASS_C
	}eLoRaWAN_MOTE_CLASS;
		
	// Enumeration to set the used back-end and use the TX and RX settings for that particular back-end.
	typedef enum
	{
		MANUAL,				//	Manual mode for setting the TX and RX channels and data rate for OTAA and data messages
		SEMTECH,			//	Semtech's the patent holder for the LoRaWAN chips.
		The_Things_Network,	//	The Things Industries back-end, which is only functional for OTAA motes. ABP is barely supported and the service may have some downtime from time-to-time for maintenance.
		KPN					//	The Dutch full-coverage LoRaWAN network provider in the Netherlands, which has LoRaWAN localization capabilities, but it's services come at a  cost.  	
	}eBACKENDS;

	/* MAC HEADER types */
	typedef enum 
	{
		JOIN_REQUEST            = 0x00, // 0000 0000
		JOIN_ACCEPT             = 0x20, // 0010 0000
		UNCONFIRMED_DATA_UP     = 0x40, // 0100 0000
		UNCONFIRMED_DATA_DOWN   = 0x60, // 0110 0000
		CONFIRMED_DATA_UP       = 0x80, // 1000 0000
		CONFIRMED_DATA_DOWN     = 0xA0, // 1010 0000 
		CONFIRMED_BITMASK		= 0x80,
		UNCONFIRMED_BITMASK		= 0x40,
		INIT_VAL				= 0x00
	}eMESSAGE_TYPES;
	
	// Receive delay enumeration for OTAA Join Delay and message delays.
	typedef enum 
	{
 		JOIN_DELAY_1   = 4900,
 		JOIN_DELAY_2   = 5900,
		RECEIVE_DELAY1 = 900,
		RECEIVE_DELAY2 = 1900	
	}eRECEIVE_DELAY;


	// Direction enumeration for the MIC calculation. See page 20, chapter 4.3.3 of the Lora Specification 1.02
	 typedef enum
	 {
		 UPSTREAM_DIR    = 0,    //  Uplink   (Away from mote)
		 DOWNSTREAM_DIR  = 1     //  Downlink (towards the mote)
	 }eDIRECTION;

	// LoRaWAN Datarates
	typedef enum
	{
		SF12_BW125kHz = 0x01,
		SF11_BW125kHz = 0x02,
		SF10_BW125kHz = 0x03,
		SF09_BW125kHz = 0x04,
		SF08_BW125kHz = 0x05,
		SF07_BW125kHz = 0x06,
		SF07_BW250kHz = 0x07,
		SF09_BW500kHz = 0x08,
		SF_BITMASK    = 0x03
	}eLoRaWAN_DATARATES;
	
	// Channel enumeration 
	typedef enum
	{
		CH00_868_100,
		CH01_868_300,
		CH02_868_500,
		CFLIST_INDEX_1,
		CFLIST_INDEX_2,
		CFLIST_INDEX_3,
		CFLIST_INDEX_4,
		CFLIST_INDEX_5,
		CH03_867_100,
		CH04_867_300,
		CH05_867_500,
		CH06_867_700,
		CH07_867_900,
		CH10_869_525
	}eLoRaWAN_CHANNELS;
	
	// Confirmed and unconfirmed messages type enumeration.
	typedef enum
	{
		UNCONFIRMED,
		CONFIRMED
	}eCONFIRMATION;
	
	// Activation methods
	typedef enum
	{
		ACTIVATION_BY_PERSONALISATION,
		OVER_THE_AIR_ACTIVATION
	}eMOTE_NETWORK_JOIN;

	//return values for the RFM95
	typedef enum
	{
		INIT,				//0 	Initialization error return code
		CRC_OK,				//1		CRC of the received message is incorrect, indicating an incomplete or corrupted message
		CRC_NOK,			//2		CRC of the received message is correct
		MIC_OK,				//3		Message Integrity Checksum is correct.
		MIC_NOK_OTAA,		//4		MIC of the OTAA message is incorrect.
		MIC_NOK_MESSAGE,	//5		MIC of the received message is incorrect.
		ADDRESS_OK,			//6		Device address of the received message matches that of this LoRaWAN mote.
		ADDRESS_NOK,		//7		The device address of the received message doesn't match to the dev address of this mote.	
		MAC_HEADER_NOK,		//8		Message header has an incorrect value, not matching any LoRaWAN headers
		RX_TIMEOUT,			//9		The RFM hasn't receive any message and got a time-out instead.
		OTAA_POINTER,		//10	Over The Air Activation structure has an incorrect pointer
		RX_MESSAGE,			//11	Message received.
		OTAA_COMPLETE,		//12	OTAA has been successfully completed.
		NEW_MESSAGE,		//13	When a new message has been received.
		DEFAULT_MAC,		//14	When a message is received with an invalid MAC header, this return value will be send back.	
		LORA_POINTER_INVALID//15	Invalid Lora pointer
	}RFM_RETVAL;



	/*********************************************************************************************
											STRUCT DEFENITIONS
	*********************************************************************************************/
	
	// Structure for both datarate and channel as they are alway used together.
	typedef struct
	{
		eLoRaWAN_DATARATES 	datarate;	
		eLoRaWAN_CHANNELS	channel;	
	}eDR_CH;
	
	// Structure for all the RFM channel settings retrieved from the OTAA connection and the default settings
	typedef struct 
	{
		uint32_t	channel[CFLIST_FREQUENCIES_MAX];
		uint8_t		index;	
		uint8_t		rx1_dr_offset;
		uint8_t		rx2_dr;
		uint8_t		rx_delay;
		bool		channel_hopping_on;
	}eCHANNEL_LIST;


	/*
		Structure used to store session data of a LoRaWAN session if ABP is used, the following parameters must be supplied by the user
		if OTAA is used, the OTAA JOIN procedure will produce the parameters for the session.
	*/
	typedef struct 
	{
		uint8_t 		NwkSKey[16];			//	The Network Session Key is used to encrypt the payload
		uint8_t 		AppSKey[16];			//	The Application Session Key is used to encrypt the payload
		uint8_t 		DevAddr[4];				//	The Device Address is used to identify the messages send and send to this Mote 
		uint16_t  		frame_counter_down;		//	Frame counter for messages received from the back-end
		uint16_t  		frame_counter_up;		//	Frame counter for messages send to the back-end.
		eRECEIVE_DELAY	Receive_delay;			//	Wait time between transmitting an confirmed LoRaWAN message and listening to the Back-end's downlink.
		uint8_t 		Transmit_Power;			//	Transmission power settings 0x00 to 0x0F.
		eDR_CH			TxChDr;					//	Transmit channel and datarate settings
		eDR_CH			RxChDr;					//	Receive channel and datarate settings
	}sLoRa_Session;

	
	/*
		Structure for connecting with Over The Air Activation to the back-end to retrieve session settings.
	*/
	typedef struct 
	{
		uint8_t 		DevEUI[8];			//	Device EUI. Unique number to identify the Mote with on the back-end.
		uint8_t 		AppEUI[16];			//	Application Key used to encrypt and calculate the session parameters with.
		uint8_t 		AppKey[16];			//	Application EUI, used to specify which Application the MOte belongs to.
		uint8_t 		DevNonce[2];		//	DevNonce is a random value that can only be used once for a join_request. The back-end will store all previously used values.	
		uint8_t 		AppNonce[3];
		uint8_t 		NetID[3];
		bool			OTAAdone;			//	Boolean for indicating if the OTAA procedure was succesfull;
		eRECEIVE_DELAY	JoinDelay;			//	Join Delay for waiting an given amount of time between the JOIN and Accept messages
		uint8_t 		Transmit_Power;		//	Transmission power settings for the OTAA connection.
		eDR_CH			TxChDr;				//	Transmit channel and datarate settings
		eDR_CH			RxChDr;				//	Receive channel and datarate settings
	}sLoRa_OTAA;

	
	// Structure to store information of a LoRaWAN message to transmit or received
	typedef struct
	{
		eMESSAGE_TYPES 	MAC_Header;				//	Message type header.
		uint8_t 		DevAddr[4];				//	Device address number used to identify the Mote with when communicating
		uint8_t 		Frame_Control;			//			
		uint16_t  		Frame_Counter;			//	Frame Counter of the message used for determining whether the message is received more than once.
		uint8_t 		Frame_Port;				//	Frame port
		uint8_t 		Frame_Options[15];		//	Array for the frame options data, if available
		uint8_t 		MIC[4];					//	Array for the calculated result for the MIC
		eCONFIRMATION	Confirmation;			//	Either it an UNCONFIRMED or CONFIRMED message up or downstream.
		RFM_RETVAL		retVal;					//	Returns status value for receiving. Indicates whether a timeout, MIC NOK or other error status occured.
		uint8_t			Count;					//	Index count for the data array
		uint8_t			Data[LORA_FIFO_SIZE];	//	Transmit and receive data array.
	}sLoRa_Message;

	
	//Structure used for storing settings of the mote
	typedef struct 
	{
		eLoRaWAN_MOTE_CLASS Mote_Class				= CLASS_A;							//	Mote Class, only CLASS A or CLASS C are supported.
		eMOTE_NETWORK_JOIN  activation_method   = ACTIVATION_BY_PERSONALISATION;
		//eMOTE_NETWORK_JOIN	activation_method		= OVER_THE_AIR_ACTIVATION;			//	Variable used to specify whether ABP or OTAA is used for activation of the mote on the back-end.
		eBACKENDS			back_end				= SEMTECH;							//	Variable to identify which back-end is used and to provide automatic OTAA and data messages channel, datarate and timeslot configuration.
		bool 				Channel_Hopping_enabled = false;							//	Enables channel hopping when set to true, the channel won't be changed when set to false		
		volatile uint16_t	timeslot;													//	Timing variable for timeslot 1 and 2.
		
		// List of additional channels in hexadecimal format pre-calculated for the RFM frequency register
		eCHANNEL_LIST	CH_list = 
		{
			.channel = {0}, 
			.index   = 0,
			.rx1_dr_offset = 0,
			.rx2_dr   = 0,
			.rx_delay = 0,
			.channel_hopping_on = true
		};
		
		/* Session parameters for the current session */
		sLoRa_Session Session =
		{
      .NwkSKey = {0x9E, 0x13, 0x34, 0xCB, 0x39, 0x39, 0xA4, 0x8B, 0x02, 0x91, 0xA4, 0xA4, 0xB8, 0x36, 0x31, 0x26},
      .AppSKey = {0xBC, 0xDC, 0x20, 0x30, 0x55, 0x09, 0xEA, 0xC9, 0x3B, 0x65, 0x19, 0xD4, 0xA5, 0xC0, 0xD8, 0x27},    
      .DevAddr = {0x26, 0x01, 0x10, 0x52},
      .frame_counter_down = 0,
			.frame_counter_up   = 0,
			.Receive_delay		= RECEIVE_DELAY2,
			.Transmit_Power     = 0x0F,
			.TxChDr = {SF09_BW125kHz, CH00_868_100},
			.RxChDr = {SF09_BW125kHz, CH10_869_525}
		};

		/* OTAA configuration and encryption parameters */
		sLoRa_OTAA OTAA =
		{
			.DevEUI   = {0},													//	If the DS2401 is used, set the DEV EUI to zero.
      .AppEUI   = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x01, 0x80, 0x13},   //  TTN Ideetron Application EUI, change this to your own APP EUI code
      .AppKey   = {0xB3, 0x23, 0xC2, 0xFD, 0xDF, 0x34, 0x7B, 0x3D, 0xA2, 0x04, 0xF2, 0x0E, 0x01, 0x3A, 0xA4, 0x59},
			.DevNonce = {0},
			.AppNonce = {0},
			.NetID    = {0},
			.OTAAdone = false,
			.JoinDelay = JOIN_DELAY_2,
			.Transmit_Power = 0x0F,
			.TxChDr = {SF10_BW125kHz, CH00_868_100},
			.RxChDr = {SF09_BW125kHz, CH10_869_525}
		};
		
		/* Message structure for both receive and transmitting */
		sLoRa_Message TX =
		{
			.MAC_Header    = INIT_VAL,
			.DevAddr       = {0},
			.Frame_Control =  0,
			.Frame_Counter =  0,
			.Frame_Port    =  0,
			.Frame_Options = {0},
			.MIC           = {0},
			.Confirmation  = CONFIRMED,
			.retVal		   = INIT,
			.Count		   =  0,
			.Data		   = {0}
		};
		sLoRa_Message RX =
		{
			.MAC_Header    = INIT_VAL,
			.DevAddr       = {0},
			.Frame_Control =  0,
			.Frame_Counter =  0,
			.Frame_Port    =  0,
			.Frame_Options = {0},
			.MIC           = {0},
			.Confirmation  = CONFIRMED,
			.retVal		   = INIT,
			.Count		   =  0,
			.Data		   = {0}
		};
	}sLoRaWAN;



#endif
