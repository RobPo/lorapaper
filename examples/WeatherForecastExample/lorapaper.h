#ifndef LORAPAPER_DEMOBOARD_H_
#define LORAPAPER_DEMOBOARD_H_

/******************************************************************************************
											DEFINES
/******************************************************************************************/

#include <SPI.h>
#include "I2C.h"
#include "spi_functions.h"
#include "progmem.h"
#include "LowPower.h"


#define EPD_RST     A2 
#define EPD_BUSY    A1  
#define EPD_CS      A3  
#define HV_BST      9  
#define SW_TFT      A0
#define DS2401      2 // One wire pin for the DS2401
#define RTC_MFP     3
#define RFM_DIO0    4
#define RFM_DIO1    5
#define RFM_DIO5    6
#define RFM_DIO2    7
#define RFM_NSS     10
#define SPI_FLASH_CS 8


/******************************************************************************************
                    STRUCTURES
******************************************************************************************/
typedef struct{
    uint8_t   Counter = 0;
    uint16_t  LoRaWAN_message_interval = 1; //  Variable to set the number of Timer2 timer ticks between LoRaWAN messages.  
}sAPP;	

#endif 
