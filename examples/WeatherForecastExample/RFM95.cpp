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
* File:     RFM95.cpp
* Author:   Gerben den Hartog
* Company: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         06-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/
#include <stdint.h>
#include <SPI.h>
#include "LoRaMAC.h"
#include "RFM95.h"
#include "spi_functions.h"
#include "Arduino.h"
#include "timers.h"
#include "lorawan_def.h"
#include "lorapaper.h"

/******************************************************************************************
* Description: Function used to initialize the RFM module on startup
******************************************************************************************/
void RFM_Init(sLoRaWAN *lora)
{
	//Switch RFM to sleep
	//DONT USE Switch mode function
	SPI_Write(RFM_NSS, 0x01, 0x00);

	//Wait until RFM is in sleep mode
	delay(10);

	//Set RFM in LoRa mode
	//DONT USE Switch mode function
	SPI_Write(RFM_NSS, 0x01, 0x80);

	//Switch RFM to standby
	RFM_Switch_Mode(0x01);

	//Set channel to channel 0 868.100 MHz
	RFM_Change_Channel(CH00_868_100, &(lora->CH_list)); 

	/*
	* Always enable the PA_BOOST pin by setting bit 7 = 1, since the RFO pin is not connected.
	*/
	RFM_Set_Output_Power(0x0F);

	//Switch LNA boost on
	SPI_Write(RFM_NSS, 0x0C, 0x23);

	//Set RFM To datarate 0 SF12 BW 125 kHz
	RFM_Change_Datarate((eLoRaWAN_DATARATES)0x00, RECEIVE_RX_TIMEOUT_MS);

	//Rx Timeout set to 37 symbols
	SPI_Write(RFM_NSS, 0x1F, 0x25);

	//Preamble length set to 8 symbols
	//0x0008 + 4 = 12
	SPI_Write(RFM_NSS, 0x20, 0x00);
	SPI_Write(RFM_NSS, 0x21, 0x08);

	//Set LoRa sync word
	SPI_Write(RFM_NSS, 0x39, 0x34);

	//Set FIFO pointers
	//Tx base address
	SPI_Write(RFM_NSS, 0x0E, 0x80);
	
	//Rx base address
	SPI_Write(RFM_NSS, 0x0F, 0x00);
	
	// Switch RFM to sleep
	SPI_Write(RFM_NSS, 0x01, 0x80);
}

/******************************************************************************************
* Description : Function for sending a package with the RFM
*
* Arguments   : *RFM_Tx_Package pointer to buffer with data and counter of data
*               *LoRa_Settings pointer to sSettings struct
******************************************************************************************/
void RFM_Send_Package(uint8_t *data, uint8_t lenght, eDR_CH *TxSettings, uint16_t TxPower, eCHANNEL_LIST *list)
{
	uint8_t i;
	uint8_t RFM_Tx_Location = 0;

	//Set RFM in Standby mode
	RFM_Switch_Mode(0x01);

	//Switch Datarate
	RFM_Change_Datarate(TxSettings->datarate, RECEIVE_RX_TIMEOUT_MS);

	//Switch Channel
	RFM_Change_Channel(TxSettings->channel, list);

	//Switch RFM_DIO0 to TxDone
	SPI_Write(RFM_NSS, 0x40, 0x40);

	//Set IQ to normal values
	SPI_Write(RFM_NSS, 0x33, 0x27);
	SPI_Write(RFM_NSS, 0x3B, 0x1D);
	
	// Set the Output Power
	RFM_Set_Output_Power(TxPower);

	//Set payload length to the right length
	SPI_Write(RFM_NSS, 0x22, lenght);

	//Get location of Tx part of FiFo
	RFM_Tx_Location = SPI_Read(RFM_NSS, 0x0E);

	//Set SPI pointer to start of Tx part in FiFo
	SPI_Write(RFM_NSS, 0x0D, RFM_Tx_Location);

	// Write the Payload to FiFo
	SPI_Write_Array(RFM_NSS, 0x00, data, lenght);

	////Set NSS pin Low to start communication
	//digitalWrite(RFM_NSS, LOW);
//
	////Send Address with MSB 1 to make it a Write command
	//SPI.transfer(0x00 | 0x80);
	//
	////Send the data array
	//SPI.transfer(data, lenght);
//
	////Set NSS pin High to end communication
	//digitalWrite(RFM_NSS, HIGH);

	
	//Switch RFM to Tx
	SPI_Write(RFM_NSS, 0x01, 0x83);

	//Wait for TxDone
	while(digitalRead(RFM_DIO0) == LOW)
	{}
	
	//Clear interrupt
	SPI_Write(RFM_NSS, 0x12, 0x08);
}

/******************************************************************************************
* Description : Function to switch RFM to single receive mode, used for Class A motes
*
* Arguments   : *LoRa_Settings pointer to sSettings struct
*
* Return	  : RFM_RETVAL Status of the received message
******************************************************************************************/
RFM_RETVAL RFM_Single_Receive(eDR_CH * RxSettings, uint16_t timeout_ms, eCHANNEL_LIST *list)
{
	RFM_RETVAL Message_Status = INIT;

	//Change DIO 0 back to RxDone
	SPI_Write(RFM_NSS, 0x40, 0x00);

	//Invert IQ Back
	SPI_Write(RFM_NSS, 0x33, 0x67);
	SPI_Write(RFM_NSS, 0x3B, 0x19);

	//Change Datarate
	RFM_Change_Datarate(RxSettings->datarate, timeout_ms);

	//Change Channel
	RFM_Change_Channel(RxSettings->channel, list);

	//Switch RFM to Single reception
	RFM_Switch_Mode(0x06);

	//Wait until RxDone or Timeout
	//Wait until timeout or RxDone interrupt
	while((digitalRead(RFM_DIO0) == LOW) && (digitalRead(RFM_DIO1) == LOW))
	{
	}

	//Check for Timeout
	if(digitalRead(RFM_DIO1) == HIGH)
	{
		//Clear interrupt register
		SPI_Write(RFM_NSS, 0x12, 0xE0);

		Message_Status = RX_TIMEOUT;
		//Serial.println("RX Timeout");
	}

	//Check for RxDone
	if(digitalRead(RFM_DIO0) == HIGH)
	{
		Message_Status = NEW_MESSAGE;
	}

	return Message_Status;
}


/******************************************************************************************
* Description : Function to switch RFM to continuous receive mode, used for Class C motes
*
* Arguments   : *LoRa_Settings pointer to sSettings struct
******************************************************************************************/
void RFM_Continuous_Receive(eDR_CH * RxSettings, eCHANNEL_LIST *list)
{
	//Change DIO 0 back to RxDone
	SPI_Write(RFM_NSS, 0x40, 0x00);

	//Invert IQ Back
	SPI_Write(RFM_NSS, 0x33, 0x67);
	SPI_Write(RFM_NSS, 0x3B, 0x19);

	//Change Datarate
	RFM_Change_Datarate(RxSettings->datarate, RECEIVE_RX_TIMEOUT_MS);

	//Change Channel
	RFM_Change_Channel(RxSettings->channel, list);

	//Switch to continuous receive
	RFM_Switch_Mode(0x05);
}

/******************************************************************************************
* Description : Function to retrieve a message received by the RFM
*
* Arguments   : *RFM_Rx_Package pointer to sBuffer struct containing the data received
*				and number of bytes received
*
* Return	  : RFM_RETVAL Status of the received message
******************************************************************************************/
RFM_RETVAL RFM_Get_Package(uint8_t *data, uint8_t *counter)
{
	uint8_t		i;
	uint8_t		RFM_Interrupts = 0;
	uint8_t		RFM_Package_Location = 0;
	RFM_RETVAL	Message_Status;

	// Get the RFM's interrupt status register value
	RFM_Interrupts = SPI_Read(RFM_NSS, 0x12);

	// UART_Send_Data(RFM_Interrupts,0x01);

	// Check if the CRC of the received package is correct.
	if((RFM_Interrupts & 0x20) != 0x20)
	{
		Message_Status = CRC_OK;
	}
	else
	{
		Message_Status = CRC_NOK;
	}

	RFM_Package_Location = SPI_Read(RFM_NSS, 0x10); 	/*Read start position of received package*/
	(*counter) = SPI_Read(RFM_NSS, 0x13); 				/*Read length of received package*/

	SPI_Write(RFM_NSS, 0x0D, RFM_Package_Location); 	/*Set SPI pointer to start of package*/
	
	// Read the FIFO from the RFM in a single read operation.
	SPI_Read_Array(RFM_NSS, 0x00, data, (*counter));
	
	//Clear interrupt register
	SPI_Write(RFM_NSS, 0x12, 0xE0);

	return Message_Status;
}




/******************************************************************************************
* Description : Function to change the datarate of the RFM module. Setting the following
*				register: Spreading factor, Bandwidth and low datarate optimization.
*
* Arguments   : Datarate the datarate to set
******************************************************************************************/
void RFM_Change_Datarate(eLoRaWAN_DATARATES Datarate, uint16_t timeout_ms)
{ 
    uint16_t TimeOutSymbols;
    
    /* Determine the new Timeout Symbol Time depending on the Datarate. */
    switch(Datarate)
	{
        //-------------------------------------------------------------------------------------------------------------
		case SF12_BW125kHz: //SF12 BW 125 kHz
            TimeOutSymbols = (timeout_ms >> 5);    // Divide the Timeout Time by 32 (Symbol time = 32.77ms)
			break;
    
        //-------------------------------------------------------------------------------------------------------------    
		case SF11_BW125kHz: //SF11 BW 125 kHz
            TimeOutSymbols = (timeout_ms >> 4);    // Divide the Timeout Time by 16 (Symbol time = 16.38ms)
			break;
            
        //-------------------------------------------------------------------------------------------------------------    
		case SF10_BW125kHz: //SF10 BW 125 kHz
            TimeOutSymbols = (timeout_ms >> 3);    // Divide the Timeout Time by 8 (Symbol time = 8.19ms)
			break;
            
        //-------------------------------------------------------------------------------------------------------------    
		case SF09_BW125kHz: //SF9 BW 125 kHz
            TimeOutSymbols = (timeout_ms >> 2);    // Divide the timeout time by 4 (Symbol time = 4.10ms)
			break;
            
        //-------------------------------------------------------------------------------------------------------------
		case SF08_BW125kHz: //SF8 BW 125 kHz
            TimeOutSymbols = (timeout_ms << 1);    // Multiply the Timeout Time by 2 (Symbol time = 2.05ms)
			break;
            
        //-------------------------------------------------------------------------------------------------------------
        default:
		case SF07_BW125kHz: //SF7 BW 125 kHz
            TimeOutSymbols = (timeout_ms);			// Copy the timeout value directly (Symbol time = 1.02ms)
    		break;
			
		//-------------------------------------------------------------------------------------------------------------
		case SF07_BW250kHz: //SF7 BW 250 kHz
			TimeOutSymbols = (timeout_ms << 1);    // Multiply the Timeout Time by 2 (Symbol time = 0.5ms)
			break;
	}
    
    /* Limit the Timeout to 1023 symbols, since the Time-out symbols register has a maximum size of 10 bits. 2^10 = 1024. */
    if(TimeOutSymbols > 1023)
    {
        TimeOutSymbols = 1023;  
    }
    
    /* Set the Datarate, CRC settings, the number of symbol for the required timeout and the Low Datarate optimizer On */
	switch(Datarate)
	{
		case SF12_BW125kHz: //SF12 BW 125 kHz
            SPI_Write(RFM_NSS, 0x1F,         (uint8_t) (TimeOutSymbols >> 0));
			SPI_Write(RFM_NSS, 0x1E, 0xC4 | ((uint8_t) (TimeOutSymbols >> 8))); //SF12, CRC On, and bits 9:8 of the Timeout Symbols
			SPI_Write(RFM_NSS, 0x1D, 0x72); //125 kHz 4/5 coding rate explicit header mode
			SPI_Write(RFM_NSS, 0x26, 0x0C); //Low datarate optimization on AGC auto on
			break;
		case SF11_BW125kHz: //SF11 BW 125 kHz
            SPI_Write(RFM_NSS, 0x1F,          (uint8_t) (TimeOutSymbols >> 0));
			SPI_Write(RFM_NSS, 0x1E, 0xB4  | ((uint8_t) (TimeOutSymbols >> 8))); //SF11 CRC On, and bits 9:8 of the Timeout Symbols
			SPI_Write(RFM_NSS, 0x1D, 0x72); //125 kHz 4/5 coding rate explicit header mode
			SPI_Write(RFM_NSS, 0x26, 0x0C); //Low datarate optimization on AGC auto on
			break;
		case SF10_BW125kHz: //SF10 BW 125 kHz
            SPI_Write(RFM_NSS, 0x1F,          (uint8_t) (TimeOutSymbols >> 0));
			SPI_Write(RFM_NSS, 0x1E, 0xA4  | ((uint8_t) (TimeOutSymbols >> 8))); //SF10 CRC On, and bits 9:8 of the Timeout Symbols
			SPI_Write(RFM_NSS, 0x1D, 0x72); //125 kHz 4/5 coding rate explicit header mode
			SPI_Write(RFM_NSS, 0x26, 0x04); //Low datarate optimization off AGC auto on
			break;
		case SF09_BW125kHz: //SF9 BW 125 kHz
            SPI_Write(RFM_NSS, 0x1F,          (uint8_t) (TimeOutSymbols >> 0));
			SPI_Write(RFM_NSS, 0x1E, 0x94  | ((uint8_t) (TimeOutSymbols >> 8))); //SF9 CRC On, and bits 9:8 of the Timeout Symbols
			SPI_Write(RFM_NSS, 0x1D, 0x72); //125 kHz 4/5 coding rate explicit header mode
			SPI_Write(RFM_NSS, 0x26, 0x04); //Low datarate optimization off AGC auto on
			break;
		case SF08_BW125kHz: //SF8 BW 125 kHz
            SPI_Write(RFM_NSS, 0x1F,          (uint8_t) (TimeOutSymbols >> 0));
			SPI_Write(RFM_NSS, 0x1E, 0x84  | ((uint8_t) (TimeOutSymbols >> 8))); //SF8 CRC On, and bits 9:8 of the Timeout Symbols
			SPI_Write(RFM_NSS, 0x1D, 0x72); //125 kHz 4/5 coding rate explicit header mode
			SPI_Write(RFM_NSS, 0x26, 0x04); //Low datarate optimization off AGC auto on
			break;
		case SF07_BW125kHz: //SF7 BW 125 kHz
            SPI_Write(RFM_NSS, 0x1F, 		  (uint8_t) (TimeOutSymbols >> 0));
			SPI_Write(RFM_NSS, 0x1E, 0x74  | ((uint8_t) (TimeOutSymbols >> 8))); //SF7 CRC On, and bits 9:8 of the Timeout Symbols
			SPI_Write(RFM_NSS, 0x1D, 0x72); //125 kHz 4/5 coding rate explicit header mode
			SPI_Write(RFM_NSS, 0x26, 0x04); //Low datarate optimization off AGC auto on
    		break;
        case SF07_BW250kHz: //SF7 BW 250kHz
            SPI_Write(RFM_NSS, 0x1F, 		  (uint8_t) (TimeOutSymbols >> 0));
			SPI_Write(RFM_NSS, 0x1E, 0x74  | ((uint8_t) (TimeOutSymbols >> 8))); //SF7 CRC On, and bits 9:8 of the Timeout Symbols
			SPI_Write(RFM_NSS, 0x1D, 0x82); //250 kHz 4/5 coding rate explicit header mode
			SPI_Write(RFM_NSS, 0x26, 0x04); //Low datarate optimization off AGC auto on
            break;
	}
}

/******************************************************************************************
* Description : Function to change the channel of the RFM module. Setting the following
*				register: Channel
* Arguments   : Channel the channel to set
******************************************************************************************/
void RFM_Change_Channel(eLoRaWAN_CHANNELS Channel, eCHANNEL_LIST *list)
{
	uint32_t hex;
	// Check if the given pointer is not invalid.
	if(list == NULL)
	{
		return;
	}
	
	switch(Channel)
	{
		default:
		case CH00_868_100: //Channel 0 868.100 MHz / 61.035 Hz = 14222987 = 0xD9068B
			SPI_Write(RFM_NSS, 0x06,0xD9);
			SPI_Write(RFM_NSS, 0x07,0x06);
			SPI_Write(RFM_NSS, 0x08,0x8B);
			break;
		case CH01_868_300: //Channel 1 868.300 MHz / 61.035 Hz = 14226264 = 0xD91358
			SPI_Write(RFM_NSS, 0x06,0xD9);
			SPI_Write(RFM_NSS, 0x07,0x13);
			SPI_Write(RFM_NSS, 0x08,0x58);
			break;
		case CH02_868_500: //Channel 2 868.500 MHz / 61.035 Hz = 14229540 = 0xD92024
			SPI_Write(RFM_NSS, 0x06,0xD9);
			SPI_Write(RFM_NSS, 0x07,0x20);
			SPI_Write(RFM_NSS, 0x08,0x24);
			break;
		case CH03_867_100: //Channel 3 867.100 MHz / 61.035 Hz = 14206603 = 0xD8C68B
			SPI_Write(RFM_NSS, 0x06,0xD8);
			SPI_Write(RFM_NSS, 0x07,0xC6);
			SPI_Write(RFM_NSS, 0x08,0x8B);
			break;
		case CH04_867_300: //Channel 4 867.300 MHz / 61.035 Hz = 14209880 = 0xD8D358
			SPI_Write(RFM_NSS, 0x06,0xD8);
			SPI_Write(RFM_NSS, 0x07,0xD3);
			SPI_Write(RFM_NSS, 0x08,0x58);
			break;
		case CH05_867_500: //Channel 5 867.500 MHz / 61.035 Hz = 14213156 = 0xD8E024
			SPI_Write(RFM_NSS, 0x06,0xD8);
			SPI_Write(RFM_NSS, 0x07,0xE0);
			SPI_Write(RFM_NSS, 0x08,0x24);
			break;
		case CH06_867_700: //Channel 6 867.700 MHz / 61.035 Hz = 14216433 = 0xD8ECF1
			SPI_Write(RFM_NSS, 0x06,0xD8);
			SPI_Write(RFM_NSS, 0x07,0xEC);
			SPI_Write(RFM_NSS, 0x08,0xF1);
			break;
		case CH07_867_900: //Channel 7 867.900 MHz / 61.035 Hz = 14219710 = 0xD8F9BE
			SPI_Write(RFM_NSS, 0x06,0xD8);
			SPI_Write(RFM_NSS, 0x07,0xF9);
			SPI_Write(RFM_NSS, 0x08,0xBE);
			break;
		case CH10_869_525: //Receive channel 869.525 MHz / 61.035 Hz = 14246334 = 0xD961BE
			SPI_Write(RFM_NSS, 0x06,0xD9);
			SPI_Write(RFM_NSS, 0x07,0x61);
			SPI_Write(RFM_NSS, 0x08,0xBE);
			break;
		case CFLIST_INDEX_1:
			if(list->index >= 1)
			{
				hex = list->channel[0];
				SPI_Write(RFM_NSS, 0x06, hex >> 16);
				SPI_Write(RFM_NSS, 0x07, hex >> 8);
				SPI_Write(RFM_NSS, 0x08, hex >> 0);
			}
			break;
			
		case CFLIST_INDEX_2:
			if(list->index >= 2)
			{
				hex = list->channel[1];
				SPI_Write(RFM_NSS, 0x06, hex >> 16);
				SPI_Write(RFM_NSS, 0x07, hex >> 8);
				SPI_Write(RFM_NSS, 0x08, hex >> 0);
			}
			break;
		case CFLIST_INDEX_3:
			if(list->index >= 3)
			{
				hex = list->channel[2];
				SPI_Write(RFM_NSS, 0x06, hex >> 16);
				SPI_Write(RFM_NSS, 0x07, hex >> 8);
				SPI_Write(RFM_NSS, 0x08, hex >> 0);
			}
			break;
		case CFLIST_INDEX_4:
			if(list->index >= 4)
			{
				hex = list->channel[3];
				SPI_Write(RFM_NSS, 0x06, hex >> 16);
				SPI_Write(RFM_NSS, 0x07, hex >> 8);
				SPI_Write(RFM_NSS, 0x08, hex >> 0);
			}
			break;
		
		case CFLIST_INDEX_5:
			if(list->index >= 5)
			{
				hex = list->channel[4];
				SPI_Write(RFM_NSS, 0x06, hex >> 16);
				SPI_Write(RFM_NSS, 0x07, hex >> 8);
				SPI_Write(RFM_NSS, 0x08, hex >> 0);
			}
			break;
	}
}


/******************************************************************************************
* Description : Function to calculate the RFM register value for the given frequency.
* Arguments   : Transmit frequency in Hertz.
******************************************************************************************/
uint32_t calculate_frequency_settings (uint32_t frequency_Hz)
{
	double calc;
	calc = (double)frequency_Hz / 61.035;
	Serial.print("Freq: ");
	Serial.print((uint32_t) frequency_Hz);
	Serial.print('\t');
	Serial.println((uint32_t) calc);
	return calc;
}

/******************************************************************************************
* Description : Function to change the operation mode of the RFM. Switching mode and wait
*				for mode ready flag
*               DO NOT USE FOR SLEEP
*
* Arguments   : Mode the mode to set
******************************************************************************************/
void RFM_Switch_Mode(uint8_t Mode)
{
	Mode |= 0x80; //Set high bit for LoRa mode

	//Switch mode on RFM module
	SPI_Write(RFM_NSS, 0x01,Mode);

	//Wait on mode ready
	while(digitalRead(RFM_DIO5) == LOW)
	{
	}
}


/******************************************************************************************
* Description : 
*	Function to change the IQ bits in the RFM95, which will allow the RFM module to send messages with the same LoRa modulation
*	as a gateway transmitter or receive message from motes just like a gateway would be able to do.
* Arguments   : Mode the mode to set
******************************************************************************************/
void RFM_SetIQ(IQ_FUNCTION function)
{
    switch(function)
    {
        //###########################################################################
        case GATEWAY_TRANSMITTER:
            // IQ set to gateway transmission
            SPI_Write(RFM_NSS, 0x33, 0x26);
            SPI_Write(RFM_NSS, 0x3B, 0x1D); 
            break;
            
        //###########################################################################
        case MOTE_TRANSCEIVER:
            // IQ set to default Mote transceiver
            SPI_Write(RFM_NSS, 0x33, 0x27);
            SPI_Write(RFM_NSS, 0x3B, 0x1D); 
            break;
            
        //###########################################################################
        default:
        case GATEWAY_RECEIVER:
            //Invert IQ for receiving messages from gateway
            SPI_Write(RFM_NSS, 0x33, 0x67);
            SPI_Write(RFM_NSS, 0x3B, 0x19);
            break;
    }
}


/**************************************************************************************************************************************************************
* Description :
*	Function to set the transmit power of the RFM95.
* Arguments   : power 0x00 - 0x0F to set the output power. Pout = 17-(15-power)
**************************************************************************************************************************************************************/
void RFM_Set_Output_Power(uint8_t power)
{
	/*
	 * Always enable the PA_BOOST pin by setting bit 7 = 1, since the RFO pin is not connected on the RFM board. Limit the output power to the maximum and set 
	 * the output power with the last 4 bits of the register.
	 */
	SPI_Write(RFM_NSS, 0x09, 0xF0 | (power & 0x0F));
}
