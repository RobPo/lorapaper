/****************************************************************************************
* This example demonstrates a simple weatherforcest demo. The supercap voltage v_scap is 
* measured minutely while the ATmega328p processor is in deep sleep all remaining time. 
* Triggering is done via external RTC to minimize current consumption during deep sleep 
* phase. IF the voltage is charged above a certain limit (ie 4.2V) and a max. number of 
* allowed Lora syncs is not exceeded, a download of is triggered from the TTN console
* and the recieved content is printed on the Epaper Screen.
/****************************************************************************************
* File:               WeatherForecastExample.ino
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
#include "LoRaMAC.h"
#include "lorawan_def.h"
#include "Cayenne_LPP.h"
#include "PL_microEPD44.h"

/******** GLOBAL VARIABLES ****************************************************************/
sAPP    app;                  // Application variable
sLoRaWAN  lora;               // See the lorapaper.h file for the settings 
sTimeDate TimeDate;           // RTC time and date variables
LORAMAC lorawan (&lora);      // Initialize the LoRaWAN stack.  
CayenneLPP LPP(&(lora.TX));   // Initialize the Low Power Protocol functions
PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);    //Initialize the EPD.

volatile bool RTC_ALARM = false;      // Interrupt variable
uint16_t v_scap, ndr, sync_max = 10;  // V_Supercap, counter of failed downloads & max_syncs

/********* INTERRUPTS *********************************************************************/
ISR(INT1_vect) {                // Interrupt vector for the alarm of the MCP7940 Real Time 
  RTC_ALARM = true;             // Clock. Do not use I2C functions or long delays
}                               // here.

ISR(TIMER1_COMPA_vect) {        // Interrupt vector for Timer1 which is used to time the Join  
  lora.timeslot++;              // and Receive windows for timeslot 1 and timelsot 2 Increment 
}                               // the timeslot counter variable for timing the

/********* MAIN ***************************************************************************/
void setup(void) {  
  analogReference(EXTERNAL);          // use AREF for reference voltage
  SPI.begin();                        // Initialize the SPI port
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  pinMode(A7, INPUT);                 // To measure V_Scap
  pinMode(SW_TFT, OUTPUT);            // Switch for V_Scap
  pinMode(DS2401, OUTPUT);            // Authenticating IC DS2401p+
  pinMode(RTC_MFP, INPUT);            // RTC MCP7940
  pinMode(RFM_DIO0, INPUT);           // RFM95W DIO0
  pinMode(RFM_DIO1, INPUT);           // RFM95W DIO1
  pinMode(RFM_DIO5, INPUT);           // RFM95W DIO5
  pinMode(RFM_DIO2, INPUT);           // RFM95W DIO2 (not really needed)
  pinMode(RFM_NSS, OUTPUT);           // RFM95W NSS = CS
  digitalWrite(DS2401, HIGH);         // to save power...
  digitalWrite(SW_TFT, HIGH);         // to save power...

  PCMSK1 = 0;                         // Disable Pin change interrupts  
  PCICR = 0;                          // Disable Pin Change Interrupts
  //delay(10);                          // Power on delay for the RFM module
  I2C_init();                         // Initialize I2C pins
  flash_power_down();                 // To save power...
  sei();                              // Enable Global Interrupts 
 
  epd.begin();                        // Turn ON & initialize 1.1" EPD
  epd.printText("Syncing with TTN...", 16, 20, 1);
  epd.update();                       // Update EPD
  epd.end();                          // to save power...
  digitalWrite(RFM_NSS, LOW);         // to save power...
  SPI.endTransaction();               // to save power...
  SPI.end();                          // to save power...

  v_scap = analogRead(A7);            // 1st Dummy-read which always delivers strange values...(to skip)
  
  mcp7940_init(&TimeDate, app.LoRaWAN_message_interval);   // Generate minutely interrupt 
}

void loop(){ 
    if(RTC_ALARM == true){             // Catch the minute alarm from the RTC. 
        RTC_ALARM = false;             // Clear the boolean.
        
        mcp7940_reset_minute_alarm(app.LoRaWAN_message_interval);        
        mcp7940_read_time_and_date(&TimeDate);    
          
        digitalWrite(SW_TFT, LOW);     // Turn ON voltage divider 
        delay(1);                      // To stabilize analogRead
        v_scap = analogRead(A7);       // Measure V_scap
        digitalWrite(SW_TFT, HIGH);    // Turn OFF voltage divider 
  
        //if (v_scap >= 320) {           // Proceed only if (Vscap > 4,2V)--> DEFAULT!
        if (v_scap >= 0) {             // Always proceed --> Use this for debugging!             
            LPP.clearBuffer();         // Form a payload according to the LPP standard to 
            LPP.addDigitalOutput(0x00, app.Counter);
            LPP.addAnalogInput(0x00, v_scap*3.3/1023*4);
            app.Counter += 1;          // Increase app.Counter

            SPI.begin();
            SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
            epd.begin(false);             // Turn ON EPD without refresh to save power
            lorawan.init();               // Init the RFM95 module

            if (app.Counter< sync_max) {  // Limit to 10 syncs to fullfill TTNs daily download limit
                lora.RX.Data[6] = 25;     // Dummy value to check lateron if downlink data was received
                lorawan.LORA_send_and_receive();
            }
                       
            if (lora.RX.Data[6] == 25)    // Downlink data received?
                ndr++;                    // if not increase nodatareceived (ndr) counter
                
             epd.printText("1Day Forecast ", 1, 2, 1);  // Following lines look a bit complex; due to 
             epd.printText(String(lora.RX.Data[3]), 107, 2, 1);   // memory constraints we can only write 
              epd.printText(":", 119, 2, 1);                      // smallfractions of the screen in one go
              if (int(lora.RX.Data[4]) < 10) {                    // generally here we have some air for 
                 epd.printText("0", 125, 2, 1);                   // improvement in the future
                 epd.printText(String(lora.RX.Data[4]), 131, 2, 1);          
              } else 
                 epd.printText(String(lora.RX.Data[4]), 125, 2, 1);      
              
              epd.printText(String(lora.RX.Data[0]), 11, 16, 3); 
              epd.printText("o", 53, 12, 2);
              epd.printText("C", 65, 16, 3);
              epd.drawBitmapLM(90, 15, wIcon_sunny, 24, 24);      // Just to demonstrate how to write little
              epd.printText(String(lora.RX.Data[1]), 11, 44, 3);  // icons; here it will always be the same 
              epd.printText("%", 65, 44, 3);                      // independent of the weather forecast :-)
              epd.printText(String(app.Counter), 124, 30, 1);
              epd.fillRectLM(90, 40, 1, (int)lora.RX.Data[6], EPD_BLACK);   // Rain probability of the next 
              epd.fillRectLM(92, 40, 1, (int)lora.RX.Data[7], EPD_BLACK);   // 12hrs... to be improved
              epd.fillRectLM(94, 40, 1, (int)lora.RX.Data[8], EPD_BLACK);
              epd.fillRectLM(96, 40, 1, (int)lora.RX.Data[9], EPD_BLACK);
              epd.fillRectLM(98, 40, 1, (int)lora.RX.Data[10], EPD_BLACK);
              epd.fillRectLM(100, 40, 1, (int)lora.RX.Data[11], EPD_BLACK);
              epd.fillRectLM(102, 40, 1, (int)lora.RX.Data[12], EPD_BLACK);
              epd.fillRectLM(104, 40, 1, (int)lora.RX.Data[13], EPD_BLACK);
              epd.fillRectLM(106, 40, 1, (int)lora.RX.Data[14], EPD_BLACK);
              epd.fillRectLM(108, 40, 1, (int)lora.RX.Data[15], EPD_BLACK);
              epd.fillRectLM(110, 40, 1, (int)lora.RX.Data[16], EPD_BLACK);
              epd.fillRectLM(112, 40, 1, (int)lora.RX.Data[17], EPD_BLACK);
              epd.printText(String(ndr), 124, 40, 1);             // Plot how many downlinks were empty
   
             epd.update();                                        // Send the framebuffer and do the update
             epd.end();                                           // To save power...
             digitalWrite(RFM_NSS, LOW);                          // To save power...
             SPI.endTransaction();                                // To save power...
             SPI.end();                                           // To save power...
        }
      } else
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);      // To save power... 
}
