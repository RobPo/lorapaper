/****************************************************************************************
* This MinimalTemplate demonstrates a simple counter, being updated everytime if there is 
* sufficient energy harvested. The supercap voltage v_scap is measured minutely while 
* the ATmega328p processor is in deep sleep all remaining time. Triggering is done via 
* external RTC to minimize current consumption during deep sleep phase. IF the voltage is 
* charged above a certain limit (ie 4.2V), an image update is triggered. 
****************************************************************************************
* File:               MinimalTemplate.ino
* Author:             Robert Poser
* Created on:         19-12-2019
* Supported Hardware: LoraPaper (with RFM95, PV cell, supercap & 1.1" EPD)
* 
* Libraries used in this sketch are based on the LoRaWAN stack from IDEETRON/NEXUS, for 
* more infos please check this great source: https://github.com/Ideetron/Nexus-Low-Power
* 
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************************/
#include "lorapaper.h"
#include "mcp7940.h"
#include "DS2401.h"
#include "spi_flash.h"
#include "RFM95.h"
#include "PL_microEPD44.h"

/******** GLOBAL VARIABLES ****************************************************************/
sTimeDate TimeDate;                             // RTC time and date variables
PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);     // Initialize the EPD.

volatile bool RTC_ALARM = false;      // Interrupt variable
uint16_t v_scap, i;                   // V_Supercap, counter of failed downloads & max_syncs

/********* INTERRUPTS *********************************************************************/
ISR(INT1_vect) {                      // Interrupt vector for the alarm of the MCP7940 Real Time 
  RTC_ALARM = true;                   // Clock. Do not use I2C functions or long delayshere
}                                     

/********* MAIN ***************************************************************************/
void setup(void) {  
  analogReference(EXTERNAL);          // use AREF for reference voltage

  pinMode(SW_TFT, OUTPUT);            // Switch for V_Scap
  pinMode(DS2401, OUTPUT);            // Authenticating IC DS2401p+
  pinMode(RFM_NSS, OUTPUT);           // RFM95W NSS = CS
  digitalWrite(DS2401, HIGH);         // to save power...
  digitalWrite(SW_TFT, HIGH);         // to save power...

  SPI.begin();                        // Initialize the SPI port
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  flash_power_down();                 // To save power...
  SPI.endTransaction();               // to save power...
  SPI.end();                          // to save power...

  v_scap = analogRead(A7);            // 1st Dummy-read which always delivers strange values...(to skip)
  I2C_init();                         // Initialize I2C pins
  mcp7940_init(&TimeDate, 1);         // Generate minutely interrupt 
}

void loop(){ 
    if(RTC_ALARM == true){             // Catch the minute alarm from the RTC. 
        RTC_ALARM = false;             // Clear the boolean.
        
        mcp7940_reset_minute_alarm(1);        
        mcp7940_read_time_and_date(&TimeDate);    
          
        digitalWrite(SW_TFT, LOW);     // Turn ON voltage divider 
        delay(1);                      // To stabilize analogRead
        v_scap = analogRead(A7);       // Measure V_scap
        digitalWrite(SW_TFT, HIGH);    // Turn OFF voltage divider 
  
        //if (v_scap >= 320) {           // Proceed only if (Vscap > 4,2V)--> DEFAULT!
        if (v_scap >= 0) {             // Always proceed --> Use this for debugging!             
            SPI.begin();
            SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
            epd.begin(false);                        // Turn ON EPD without refresh to save power
            SPI_Write(RFM_NSS, 0x01, 0x00);          // Switch RFM to sleep
            epd.printText("Hello World!", 1, 2, 1);  // Following lines look a bit complex; due to 
            epd.printText(String(++i), 20, 20, 3);
            epd.drawBitmapLM(90, 15, wIcon_sunny, 24, 24);       // Just to demonstrate how to write little
            epd.update();                                        // Send the framebuffer and do the update
            epd.end();                                           // To save power...
            digitalWrite(RFM_NSS, LOW);                          // To save power...
            SPI.endTransaction();                                // To save power...
            SPI.end();                                           // To save power...
        }
      } else
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);      // To save power... 
}
