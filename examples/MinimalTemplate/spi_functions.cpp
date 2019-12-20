#include <stdint.h>
#include <SPI.h>
#include "LoRaMAC.h"
#include "RFM95.h"
#include "Arduino.h"
#include "timers.h"
#include "lorawan_def.h"
#include "spi_functions.h"

/******************************************************************************************
* Description : Function that reads a register and returns the value
* Arguments   : Address of register to be read
* Returns   : Value of the register
******************************************************************************************/

uint8_t SPI_Read(uint8_t CS_pin, uint8_t register_Address) {
	uint8_t RFM_Data;
	
	digitalWrite(CS_pin, LOW);        // Set NSS pin low to start SPI communication
	SPI.transfer(register_Address);   // Send Address
	RFM_Data = SPI.transfer(0x00);    // Send 0x00 to be able to receive the answer from the RFM
	digitalWrite(CS_pin, HIGH);       // Set NSS high to end communication
	return RFM_Data;                  // Return received data
}

/******************************************************************************************
* Description: Function that writes a register with the given value
* Arguments:   Address of the register to be written
* Data         Data to be written to the register
******************************************************************************************/
void SPI_Write(uint8_t CS_pin, uint8_t register_Address, uint8_t Data) {
	digitalWrite(CS_pin, LOW);              //Set NSS pin Low to start communication
  SPI.transfer(register_Address | 0x80);  //Send Address with MSB 1 to make it a writ command
	SPI.transfer(Data);                     //Send Data
  digitalWrite(CS_pin, HIGH);             //Set NSS pin High to end communication
}


/**********************************************************************************************
* Description: Function that writes an array of data to the RFM register
* Arguments:   Address of the register to be written to.
* Data:		     Pointer to the Data array to be written to, starting on to the given register address
* Lenght:		   The number of bytes needed to transmit
**********************************************************************************************/
void SPI_Write_Array(uint8_t CS_pin, uint8_t register_Address, uint8_t *Data, uint8_t lenght) {
	digitalWrite(CS_pin, LOW);                //Set NSS pin Low to start communication
	SPI.transfer(register_Address | 0x80);    //Send Address with MSB 1 to make it a Write command
	SPI.transfer(Data, lenght);               //Send the data array
	digitalWrite(CS_pin, HIGH);               //Set NSS pin High to end communication
}

/*****************************************************************************************************
* Description: Function that writes an array of data to the RFM register
* Arguments:   Address of the RFM register to be written to.
* Data:		     Pointer to the Data array to be written to, starting on to the given register address
* lenght:		   The number of bytes needed to transmit
*****************************************************************************************************/
void SPI_Read_Array(uint8_t CS_pin, uint8_t register_Address, uint8_t *Data, uint8_t lenght) {
	digitalWrite(CS_pin, LOW);            //Set Chip select pin low to start SPI communication
	SPI.transfer(register_Address);       //Send the register Address and then read the contents of the receive buffer in the RFM
	SPI.transfer(Data, lenght);	          //Set NSS high to end communication
	digitalWrite(CS_pin, HIGH);
}
