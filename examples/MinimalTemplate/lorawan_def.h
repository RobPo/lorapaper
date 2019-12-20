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
      //.NwkSKey = {0xA0, 0x5B, 0x04, 0x27, 0xBC, 0xE8, 0x68, 0x61, 0xFC, 0xE3, 0xC9, 0x82, 0x43, 0x22, 0x50, 0x95},
			//.NwkSKey = {0x46, 0xB9, 0x00, 0x68, 0x46, 0xA6, 0x38, 0x38, 0xC0, 0x1E, 0xA1, 0x89, 0x98, 0xA8, 0x64, 0x49},
			//.NwkSKey = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C},
      //.NwkSKey = {0x9D, 0x89, 0x88, 0x07, 0x44, 0x80, 0xAD, 0x9B, 0xF1, 0x3E, 0x7D, 0x7D, 0xF8, 0x6C, 0x97, 0xA3},
		  //.NwkSKey = {0xB6, 0xB7, 0x01, 0x1D, 0xC7, 0x5F, 0x6A, 0x8B, 0x27, 0x36, 0xD9, 0xEE, 0x18, 0x29, 0x10, 0x03},
      //.NwkSKey = {0xC6, 0x0E, 0x1B, 0x62, 0x21, 0x72, 0xDF, 0x58, 0x2C, 0x94, 0x60, 0xE0, 0xA8, 0x3F, 0xC9, 0xFA},
      //.NwkSKey = {0x5F, 0xFE, 0xD7, 0xCD, 0x39, 0xCB, 0xA3, 0x05, 0x81, 0x6D, 0x66, 0x51, 0x98, 0x72, 0x41, 0xB1},
      //.NwkSKey = {0xB7, 0x95, 0x61, 0xDB, 0x1C, 0xF9, 0x3C, 0x8D, 0xBF, 0xF2, 0x7D, 0x8E, 0x61, 0xD8, 0x70, 0x6F},
      //.NwkSKey = {0x70, 0x12, 0x46, 0x98, 0xC5, 0xE0, 0x11, 0x7C, 0xD4, 0x5C, 0x32, 0xCE, 0xA2, 0x11, 0x07, 0x15},
      //.NwkSKey = {0x76, 0x9A, 0x6E, 0x60, 0xA5, 0xC0, 0xE3, 0xC9, 0x8F, 0x78, 0xB0, 0x89, 0xCF, 0xC1, 0x3D, 0xE4},
      //.NwkSKey = {0x7D, 0xA6, 0x9F, 0xD0, 0x9C, 0xCC, 0x72, 0x82, 0x06, 0x62, 0x90, 0x59, 0x15, 0x84, 0x29, 0x4C},
      //.NwkSKey = {0xC4, 0x35, 0x99, 0xB7, 0xD9, 0x6E, 0x85, 0x62, 0x4A, 0x1A, 0x75, 0x2F, 0x14, 0xF9, 0x49, 0x6B},
      //.NwkSKey = {0x7C, 0x47, 0xCA, 0xAD, 0xC5, 0x26, 0x41, 0x19, 0xEB, 0x16, 0x07, 0xED, 0xE6, 0xB6, 0x6D, 0xC8},
      //.NwkSKey = {0x38, 0x53, 0xD1, 0xF5, 0x58, 0x74, 0xD0, 0xC1, 0x93, 0x76, 0x81, 0x8B, 0x64, 0xC9, 0xF9, 0x1C},
      //.NwkSKey = {0xA6, 0x5F, 0xEF, 0xC0, 0x47, 0xB1, 0xC2, 0xD0, 0x28, 0x40, 0x0D, 0xDE, 0xCE, 0xA4, 0xDC, 0x2F},
      //.NwkSKey = {0x83, 0x62, 0xC7, 0xE5, 0xA3, 0xC9, 0x4A, 0x33, 0x0A, 0x43, 0xC3, 0xF9, 0x4E, 0x15, 0x9E, 0x5D},
      //.NwkSKey = {0xC1, 0x01, 0xF2, 0x71, 0x2C, 0x60, 0xB8, 0x0C, 0x46, 0xA9, 0x8C, 0xA4, 0x87, 0x45, 0x40, 0x7D},
      //.NwkSKey = {0x78, 0x87, 0x29, 0x2F, 0x32, 0x9F, 0x4F, 0xBE, 0x45, 0x1D, 0x34, 0xB3, 0x5D, 0xE1, 0xFA, 0x71},
      //.NwkSKey = {0x45, 0x1A, 0xEE, 0x9C, 0x30, 0xC5, 0xAA, 0x8C, 0x2D, 0xB4, 0xA4, 0xEB, 0x37, 0xE2, 0x0B, 0x9B},
      //.NwkSKey = {0x40, 0x6A, 0xB7, 0x95, 0x52, 0x26, 0xB2, 0x10, 0x31, 0x15, 0x11, 0x03, 0xAC, 0x08, 0xB3, 0x36},
      .NwkSKey = {0x9E, 0x13, 0x34, 0xCB, 0x39, 0x39, 0xA4, 0x8B, 0x02, 0x91, 0xA4, 0xA4, 0xB8, 0x36, 0x31, 0x26},
      .AppSKey = {0xBC, 0xDC, 0x20, 0x30, 0x55, 0x09, 0xEA, 0xC9, 0x3B, 0x65, 0x19, 0xD4, 0xA5, 0xC0, 0xD8, 0x27},    
      //.AppSKey = {0x89, 0xE8, 0x41, 0xDA, 0x62, 0xE7, 0x50, 0xCA, 0xBE, 0xEF, 0x2A, 0xAA, 0x19, 0x31, 0xF9, 0x25},    
      //.AppSKey = {0xDD, 0x09, 0x11, 0x07, 0x73, 0x04, 0xE5, 0x9D, 0xE6, 0x3A, 0x24, 0xE7, 0x9E, 0x25, 0xF4, 0x2F},    
      //.AppSKey = {0x6C, 0x61, 0x57, 0xE8, 0xF4, 0x0A, 0x7B, 0xE0, 0x8F, 0x71, 0x7B, 0x07, 0x01, 0x30, 0x58, 0xC5},    
      //.AppSKey = {0x0B, 0xF2, 0x13, 0x20, 0xCC, 0xB0, 0x99, 0xF3, 0xBE, 0x0B, 0x96, 0xCE, 0x59, 0x66, 0x98, 0x94},   
      //.AppSKey = {0x2F, 0x79, 0x4C, 0x52, 0xC1, 0x93, 0x10, 0x2D, 0xAC, 0x4F, 0x03, 0x4D, 0x87, 0x0D, 0x0A, 0xA8},   
      //.AppSKey = {0xFB, 0xC3, 0x0C, 0x40, 0xDF, 0x26, 0xE7, 0x94, 0x0A, 0x8A, 0x04, 0x95, 0x75, 0x4D, 0xCB, 0x02},   
      //.AppSKey = {0xAD, 0x90, 0x26, 0xF7, 0xFF, 0x85, 0xA3, 0x32, 0x76, 0x61, 0x10, 0x5B, 0xE8, 0x16, 0x75, 0xCC},    
      //.AppSKey = {0xD5, 0x9B, 0x18, 0xB0, 0xB6, 0x7F, 0x60, 0x7D, 0xE9, 0x23, 0x4C, 0x70, 0x6F, 0xB5, 0xFF, 0xC1},    
      //.AppSKey = {0xCC, 0x49, 0x22, 0xDE, 0xEE, 0xDA, 0xC9, 0x57, 0x2B, 0x5E, 0xD6, 0x23, 0x69, 0xF5, 0xBF, 0xC6},    
      //.AppSKey = {0x20, 0xC5, 0x79, 0x9F, 0x6E, 0xE8, 0x51, 0x51, 0x5C, 0x5D, 0x27, 0xA4, 0x8B, 0x3D, 0x99, 0x67},    
      //.AppSKey = {0x0F, 0x0A, 0xD0, 0x0A, 0x8A, 0x57, 0x1B, 0x28, 0xF9, 0xED, 0xB4, 0xBB, 0x40, 0xD1, 0x11, 0xC8},    
      //.AppSKey = {0x68, 0xFE, 0xB9, 0x8D, 0x78, 0x96, 0x51, 0x8C, 0x45, 0xD5, 0x67, 0xB9, 0x4B, 0x00, 0x27, 0x4C},
      //.AppSKey = {0x24, 0x47, 0x0E, 0xF6, 0xC5, 0xCD, 0x34, 0x9F, 0xAD, 0xED, 0x30, 0xEB, 0xC6, 0x02, 0xBC, 0x2F},
      //.AppSKey = {0xE6, 0x3E, 0x71, 0x0D, 0x49, 0x2E, 0xBB, 0x60, 0x6F, 0x04, 0x8E, 0xB3, 0x71, 0xFA, 0x3A, 0x89},
      //.AppSKey = {0x8C, 0xA4, 0x33, 0x5C, 0x0C, 0x56, 0x6B, 0x56, 0x47, 0x7B, 0x05, 0xD7, 0xCC, 0x11, 0xC1, 0x98},
      //.AppSKey = {0x48, 0x24, 0x9C, 0x8A, 0x25, 0xFF, 0xD2, 0xA4, 0xAB, 0xF6, 0xC0, 0x36, 0x73, 0x3B, 0x33, 0xB9},
     	//.AppSKey = {0x3E, 0x36, 0x9A, 0x80, 0x93, 0x9A, 0xAD, 0x8B, 0x61, 0xDA, 0x3D, 0xE1, 0xE8, 0x1F, 0x0A, 0x0A},
     	//.AppSKey = {0xA7, 0x55, 0xF5, 0xF7, 0x25, 0x3B, 0x98, 0x7E, 0x3A, 0x68, 0xA1, 0x68, 0x95, 0x97, 0xD4, 0x65},
		  //.AppSKey = {0xE6, 0x5E, 0xCF, 0x63, 0x06, 0x79, 0x3E, 0xA2, 0x09, 0x35, 0xA7, 0x28, 0x88, 0x69, 0x82, 0xDB},
		  //.AppSKey = {0x15, 0x0F, 0x46, 0x45, 0x0F, 0xB5, 0x76, 0xF8, 0xBA, 0xA2, 0x1D, 0xFB, 0x3D, 0xA5, 0x84, 0x81},
    	//.DevAddr = {0x3A, 0x12, 0x34, 0x56},
			//.DevAddr = {0x26, 0x01, 0x14, 0xF4},
			//.DevAddr = {0x26, 0x01, 0x15, 0x16},
      //.DevAddr = {0x26, 0x01, 0x13, 0x4F},
      //.DevAddr = {0x26, 0x01, 0x1E, 0x04},
      //.DevAddr = {0x26, 0x01, 0x17, 0x9F},
      //.DevAddr = {0x26, 0x01, 0x15, 0xE3},
      //.DevAddr = {0x26, 0x01, 0x1B, 0x47},
      //.DevAddr = {0x26, 0x01, 0x1E, 0xDA},
      //.DevAddr = {0x26, 0x01, 0x13, 0x27},
      //.DevAddr = {0x26, 0x01, 0x1C, 0x3A},
      //.DevAddr = {0x26, 0x01, 0x18, 0xE4},
      //.DevAddr = {0x26, 0x01, 0x10, 0x0F},
      //.DevAddr = {0x26, 0x01, 0x15, 0x01},
      //.DevAddr = {0x26, 0x01, 0x12, 0x0B},
      //.DevAddr = {0x26, 0x01, 0x11, 0x67},
      //.DevAddr = {0x26, 0x01, 0x1E, 0x8C},
      //.DevAddr = {0x26, 0x01, 0x1C, 0x25},
      //.DevAddr = {0x26, 0x01, 0x1C, 0x6F},
      .DevAddr = {0x26, 0x01, 0x10, 0x52},
      //.DevAddr = {0x26, 0x01, 0x18, 0xAC},
      //.DevAddr = {0x26, 0x01, 0x1D, 0x67},
      //.DevAddr = {0x26, 0x01, 0x1A, 0xCA},
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
      //.AppEUI   = {0x01, 0xF7, 0x03, 0x0E, 0x19, 0x00, 0x00, 0x55},   //  TTN Ideetron Application EUI, change this to your own APP EUI code
      //.AppKey   = {0x0B, 0x32, 0x3C, 0x2F, 0xDD, 0xF3, 0x47, 0xB3, 0xDA, 0x20, 0x4F, 0x20, 0xE0, 0x13, 0xAA, 0x45},
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
