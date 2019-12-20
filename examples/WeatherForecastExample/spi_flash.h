// spi_flash.h

#ifndef _SPI_FLASH_h
#define _SPI_FLASH_h

	/*********************************************************************************************
											INCLUDES
	*********************************************************************************************/
	#include <stdint.h>


	/*********************************************************************************************
											DEFINITIONS
	*********************************************************************************************/	
	#define SPI_FLASH_PAGE_SIZE	256

	/******************************************************************************************
											ENUMERATIONS
	******************************************************************************************/	
	typedef enum
	{
		FLASH_WRITE_ENABLE		= 0x06,
		FLASH_WRITE_EN_VOLATILE = 0x50,
		FLASH_WRITE_DISABLE		= 0x04,
		FLASH_READ_STATUS		= 0x05,
		FLASH_WRITE_STATUS		= 0x01,
		FLASH_READ_DATA			= 0x03,
		FLASH_FAST_READ			= 0x0B,
		FLASH_FAST_READ_DUAL	= 0x3B,
		FLASH_READ_DUAL_IO		= 0xBB,
		FLASH_PAGE_PROGRAM		= 0x02,
		FLASH_SECTOR_ERASE		= 0x20,
		FLASH_BLOCK_ERASE_32K	= 0x52,
		FLASH_BLOCK_ERASE_64K	= 0xD8,
		FLASH_CHIP_ERASE		= 0xC7,
		FLASH_CHIP_ERASE2		= 0x60,
		FLASH_POWER_DOWN		= 0xB9,
		FLASH_RELEASE_POWER_DOWN= 0xAB,
		FLASH_MANUFACTURERE_ID	= 0x90,
		FLASH_ID_DUAL_IO		= 0x92,
		FLASH_JEDEC_ID			= 0x9F,
		FLASH_READ_UNIQUE_ID	= 0x4B		
	}eFLASH_COMMANDS;
	
	typedef enum
	{
		REGISTER_PROTECT	= 0x80,
		TOP_BOT_PROTECT		= 0x20,
		BLOCK_PROTECT		= 0x1C,
		WRITE_ENABLE_LATCH	= 0x02,
		R_W_IN_PROGRESS		= 0x01
	}eSTATUS_BITS;
	
	/******************************************************************************************
											STRUCTURE
	******************************************************************************************/
	typedef struct
	{
		uint8_t	 deviceID;
		uint8_t	 manufacturerID;
	}sFLASH_ID;


	/******************************************************************************************
										FUNCTION PROTOTYPES
	******************************************************************************************/
	void	flash_ID					(sFLASH_ID * ID);
	void	flash_power_down			(void);
	void	flash_release_power_down	(void);
	void	flash_write					(uint32_t address, uint8_t *data, uint16_t n);
	void	flash_read					(uint32_t address, uint8_t *data, uint16_t n);
	void	flash_write_enable			(void);
	void	flash_write_disable			(void);
	uint8_t flash_status				(void);

#endif
