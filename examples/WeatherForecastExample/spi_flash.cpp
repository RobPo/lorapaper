#include "Arduino.h"
#include <SPI.h>
#include "spi_functions.h"
#include "spi_flash.h"
#include "lorapaper.h"


void flash_ID (sFLASH_ID * ID)
{
	if(ID == NULL)
	{
		return;
	}
	
	flash_release_power_down();
	
	//Set CS pin low
	digitalWrite(SPI_FLASH_CS, LOW);

	//Send instruction
	SPI.transfer(FLASH_MANUFACTURERE_ID); // 0x90
	
	//Send 0x000000 for address
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);

	//Get Manufacture ID
	ID->manufacturerID = SPI.transfer(0x00);

	//Get device ID
	ID->deviceID = SPI.transfer(0x00);

	//Set CS pin HIGH
	digitalWrite(SPI_FLASH_CS, HIGH);
}


void flash_power_down (void)
{
	// Wait for the SPI flash IC to be ready for further instructions.
	while(flash_status() & R_W_IN_PROGRESS)
	{}
	
	//Set CS pin low
	digitalWrite(SPI_FLASH_CS, LOW);
	
	//Send instruction
	SPI.transfer(FLASH_POWER_DOWN); // 0xB9
	
	digitalWrite(SPI_FLASH_CS, HIGH);
}

void flash_release_power_down (void)
{
	//Set CS pin low
	digitalWrite(SPI_FLASH_CS, LOW);
	
	//Send instruction and four dummy bytes
	SPI.transfer(FLASH_RELEASE_POWER_DOWN); // 0xAB
	SPI.transfer(0x00);  // Dummy
	SPI.transfer(0x00);  // Dummy
	SPI.transfer(0x00);  // Dummy
	SPI.transfer(0x00);  // ID7:ID0 will repeat this until SPI transfer has finished.
	
	digitalWrite(SPI_FLASH_CS, HIGH);
	
	// Wait for the SPI flash IC to be ready for further instructions.
	while(flash_status() & R_W_IN_PROGRESS)
	{}
}

void flash_write(uint32_t address, uint8_t *data, uint16_t n)
{
	uint8_t i;
	
	flash_release_power_down();
	
	// Enable writing to the Flash Chip
	flash_write_enable();
	
	//delay(1);

	// Set CS low 
	digitalWrite(SPI_FLASH_CS, LOW);

	// Send page program instruction
	SPI.transfer(FLASH_PAGE_PROGRAM); // 0x02

	// Send address
	SPI.transfer((uint8_t) (address >> 16));
	SPI.transfer((uint8_t) (address >>  8));
	SPI.transfer((uint8_t) (address >>  0));

	// Write DS bytes
	for(i = 0 ; i < n ; i++)
	{
		//SPI.transfer(i); // writes the value of i to the 
		SPI.transfer(data[i]);
	}

	// Set Cs pin high
	digitalWrite(SPI_FLASH_CS, HIGH);
	
	//delay(2000);
	// Wait for the SPI flash IC to be ready for further instructions.
	while(flash_status() & R_W_IN_PROGRESS)
	{}
}

void flash_read(uint32_t address, uint8_t *data, uint16_t n)
{
	uint16_t i;
	
	flash_release_power_down();
	
	// Set CS low again
	digitalWrite(SPI_FLASH_CS, LOW);

	// Send page program instruction
	SPI.transfer(FLASH_READ_DATA); // 0x03

	// Send address
	SPI.transfer((uint8_t) (address >> 16));
	SPI.transfer((uint8_t) (address >>  8));
	SPI.transfer((uint8_t) (address >>  0));

	// Write DS bytes
	for(i = 0 ; i < n ; i++)
	{
		data[i] = SPI.transfer(0x00);
	}

	// Set Cs pin high
	digitalWrite(SPI_FLASH_CS, HIGH);
}

void flash_write_enable (void)
{
	// Set write enable latch
	digitalWrite(SPI_FLASH_CS, LOW);

	// Send Write enable latch instruction
	SPI.transfer(FLASH_WRITE_ENABLE); // 0x06

	// Set CS high again
	digitalWrite(SPI_FLASH_CS, HIGH);
}

void flash_write_disable (void)
{
	// Set write enable latch
	digitalWrite(SPI_FLASH_CS, LOW);

	// Send Write enable latch instruction
	SPI.transfer(FLASH_WRITE_DISABLE); // 0x04

	// Set CS high again
	digitalWrite(SPI_FLASH_CS, HIGH);	
}


uint8_t flash_status (void)
{
	uint8_t status = 0;
	
	// Set write enable latch
	digitalWrite(SPI_FLASH_CS, LOW);

	// Send Write enable latch instruction
	SPI.transfer(FLASH_READ_STATUS); // 0x05
	
	// Read the status byte
	status = SPI.transfer(0x00);

	// Set CS high again
	digitalWrite(SPI_FLASH_CS, HIGH);
	
	return status;
}
