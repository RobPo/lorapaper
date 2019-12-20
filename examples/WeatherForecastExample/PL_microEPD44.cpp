/* *****************************************************************************************
PL_MICROEPD - A Universal Hardware Library for 1.1”, 1.4", 2.1" and 3.1" E-Paper displays 
(EPDs) from Plastic Logic based on UC8156 driver IC. The communication is SPI-based, for more
information about hook-up and tutorials please check: https://github.com/RobPo/Paperino.

Created by Robert Poser, Feb 9th 2018, Dresden/Germany. Released under BSD license
(3-clause BSD license), check license.md for more information.

We invested time and resources providing this open source code, please support Paperino and
open source hardware by purchasing this product @Crowd_supply @Watterott @Plasticlogic
***************************************************************************************** */
#include "LowPower.h"
#include "PL_microEPD44.h"
#include <SPI.h>
#include "spi_functions.h"

PL_microEPD::PL_microEPD(int8_t _cs, int8_t _rst, int8_t _busy) : Adafruit_GFX(EPD_WIDTH, 
EPD_HEIGHT) {
    cs      = _cs;
    rst     = _rst;
    busy    = _busy;
}

// PUBLIC

// ******************************************************************************************
// BEGIN - Resetting UC8156 driver IC and configuring all sorts of behind-the-scenes-settings
// By default (WHITEERASE=TRUE) a clear screen update is triggered once to erase the screen.
// ******************************************************************************************
void PL_microEPD::begin(bool whiteErase) {
    pinMode(cs, OUTPUT);  
    pinMode(busy, INPUT);
    pinMode(rst, OUTPUT);   

    delay(10);

    digitalWrite(rst, HIGH);  
    delay(5);   
    pinMode(9, OUTPUT);                  // VDD for EPD
    digitalWrite(9, HIGH);               // Enable VDD for UC8156
    delay(5);
    digitalWrite(rst, LOW);                 // Trigger global reset
    delay(5);        
    digitalWrite(rst, HIGH);
    delay(5); 

    delay(10);
    
    waitForBusyInactive();
    
    _width=24; _height=24; nextline= _width/4; _buffersize=_width*_height/4;
    _EPDwidth=148; _EPDheight=72;
    writeRegister(EPD_PANELSETTING, 0x12, -1, -1, -1);        
    writeRegister(EPD_WRITEPXRECTSET, 0, 31, 0, 23);
    writeRegister(EPD_VCOMCONFIG, 0x00, 0x00, 0x24, 0x07);
    writeRegister(EPD_DRIVERVOLTAGE, 0x25, 0xFF, -1, -1);   //Vgate=+17V/-25V, Vsource=+/-15V
    writeRegister(EPD_BORDERSETTING, 0x04, -1, -1, -1);
    writeRegister(EPD_LOADMONOWF, 0x60, -1, -1, -1);
    writeRegister(EPD_INTTEMPERATURE, 0x0A, -1, -1, -1);
    writeRegister(EPD_BOOSTSETTING, 0x22, 0x37, -1, -1);      // DC 30%/30% (Normal/Softstart), 125KHz, 16384 CC
    writeRegister(EPD_DATENTRYMODE, 0x27, -1, -1, -1);        
    setTextColor(EPD_BLACK);                    //Set text color to black as default

   if (whiteErase) 
       WhiteErase();                             //Start with a white refresh if TRUE
    clear();
}

// ************************************************************************************
// CLEAR - Erases the image buffer and triggers an image update and sets the cursor
// back to the origin coordinates (0,0).
// ************************************************************************************
void PL_microEPD::clearBuffer() {
    memset(buffer, 0xFF, sizeof(buffer));
    setCursor(0,0);
}

void PL_microEPD::clear(int c) {
    writeRegister(EPD_WRITEPXRECTSET, 0, 71, 0, 147);
    digitalWrite(cs, LOW);
    SPI.transfer(0x10);
    if (c==EPD_BLACK)
      for (int x=0; x < _EPDwidth*_EPDheight/4; x++)
          SPI.transfer(0x00);
    else
      for (int x=0; x < _EPDwidth*_EPDheight/4; x++)
          SPI.transfer(0xFF);
    digitalWrite(cs, HIGH);
    writeRegister(EPD_WRITEPXRECTSET, 0, _height-1, 0, _width-1);
    clearBuffer();
}

void PL_microEPD::setCursorSegment(int x, int y) {
    if (x < (_EPDwidth - _width) && y < (_EPDheight - _height)) {    
        writeRegister(EPD_WRITEPXRECTSET, y, y + _height-1, x, x + _width-1);
        writeRegister(EPD_PIXELACESSPOS, y, x, -1, -1);       
    } else {
        writeRegister(EPD_WRITEPXRECTSET, 0, _height-1, 0, _width-1);
        writeRegister(EPD_PIXELACESSPOS, 0, 0, -1, -1);       
    }
}

// ************************************************************************************
// DRAWPIXEL - Draws pixel in the memory buffer at position X, Y with the value of the
// parameter color (2 bit value).
// ************************************************************************************
void PL_microEPD::drawPixel(int16_t x, int16_t y, uint16_t color) {

    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height) || (color>4 )) return;  
    x+=2;
    uint8_t pixels = buffer[x/4 + (y) * nextline];
    switch (x%4) {                      //2-bit grayscale dot
      case 0: buffer[x/4 + (y) * nextline] = (pixels & 0x3F) | ((uint8_t)color << 6); break;  
      case 1: buffer[x/4 + (y) * nextline] = (pixels & 0xCF) | ((uint8_t)color << 4); break;  
      case 2: buffer[x/4 + (y) * nextline] = (pixels & 0xF3) | ((uint8_t)color << 2); break;  
      case 3: buffer[x/4 + (y) * nextline] = (pixels & 0xFC) | (uint8_t)color; break;   
  }
}

// ************************************************************************************
// UPDATE - Triggers an image update based on the content written in the image buffer.
// There are three different updateModes supported: EPD_UPD_FULL(0) is set by default,
// achieves four greyelevels, takes about 800ms and refreshes all pixels. This is the
// update mode having the best image quality. EPD_UPD_PART(1) is a variant of the
// previous one but only changing pixels are refreshed. This results in less flickering
// for the price of a slightly higher pixel to pixel crosstalk. EPD_UPD_MONO(2) is
// again a variant of the previous update mode but only about 250ms long. this allows
// slightly faster and more responsive updates for the price of only two greylevels
// being supported (EPD_BLACK and EPD_WHITE). Depending on your application it is
// recommended to insert a full update EPD_UPD_FULL(0) after a couple of mono updates
// to increase the image quality.
// THIS KIND OF DISPLAY IS NOT SUITED FOR LONG RUNNING ANIMATIONS OR APPLICATIONS WITH
// CONTINUOUSLY HIGH UPDATE RATES. AS A RULE OF THUMB PLEASE TRIGGER UPDATES IN AVERAGE
// NOT FASTER THAN MINUTELY (OR RUN BACK2BACK UPDATES NOT LONGER AS ONE HOUR PER DAY.)
// ************************************************************************************
void PL_microEPD::update(int updateMode) {
    if (updateMode==3)
      powerOn(0);
      //powerOn(1);
    else
      powerOn();
    switch (updateMode) {
        case 0:
            writeRegister(EPD_DRIVERVOLTAGE, 0x25, 0xFF, -1, -1);   //Vgate=+17V/-25V, Vsource=+/-15V
            writeRegister(EPD_PROGRAMMTP, 0x00, -1, -1, -1);
            writeRegister(EPD_DISPLAYENGINE, 0x03, -1, -1, -1);
            break;
        case 2:
            writeRegister(EPD_DRIVERVOLTAGE, 0x25, 0xFF, -1, -1);       //Vgate=+17V/-25V, Vsource=+/-15V
            writeRegister(EPD_PROGRAMMTP, 0x02, -1, -1, -1);            //Use mono waveform
            writeRegister(EPD_DISPLAYENGINE, 0x03, -1, -1, -1);         //Only changing pixels updated
            break;
        case 3:
            writeRegister(EPD_DRIVERVOLTAGE, 0x25, 0xFF, -1, -1);       //Vgate=+15V/-20V, Vsource=+/-12V
            writeRegister(EPD_PROGRAMMTP, 0x02, -1, -1, -1);            //Use short mono waveform
            writeRegister(EPD_DISPLAYENGINE, 0x03, -1, -1, -1);   //Only changing pixels updated (delta mode)
            //writeRegisterNoWait(EPD_DISPLAYENGINE, 0x03, -1, -1, -1);   //Only changing pixels updated (delta mode)
            //writeRegister(EPD_DRIVERVOLTAGE, 0x00, 0x88, -1, -1);       //Vgate=+15V/-20V, Vsource=+/-12V
            //writeRegister(EPD_PROGRAMMTP, 0x02, -1, -1, -1);            //Use short mono waveform
            //writeRegisterNoWait(EPD_DISPLAYENGINE, 0x07, -1, -1, -1);   //Only changing pixels updated (delta mode)
            LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
            LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
            break;
    }
    powerOff();
}


// ************************************************************************************
// SETVBORDERCOLOR - Sets the color of the VBorder around the active area. By default
// this is set to White (matching to the Paperino FrontCover) and should not be changed
// ************************************************************************************
void PL_microEPD::setVBorderColor(int color) {
    if (color==3) writeRegister(EPD_BORDERSETTING, 0xF7, -1, -1, -1);  //
    if (color==0) writeRegister(EPD_BORDERSETTING, 0x07, -1, -1, -1);  //
    update(EPD_UPD_PART);
    writeRegister(EPD_BORDERSETTING, 0x04, -1, -1, -1);  //
}

// PRIVATE

// ************************************************************************************
// POWERON - Activates the defined high voltages needed to update the screen. The
// command should always be called before triggering an image update.
// ************************************************************************************
void PL_microEPD::powerOn(bool lowpower) {
    waitForBusyInactive();
    writeRegister(EPD_SETRESOLUTION, 0, 239, 0, 147);
    writeRegister(EPD_TCOMTIMING, 0x67, 0x55, -1, -1);        // GAP=115µs; S2G=G2S=85µs @1MHz
    writeRegister(EPD_POWERSEQUENCE, 0x00, 0x00, 0x00, -1);
    if (lowpower)
        writeRegister(EPD_POWERCONTROL, 0x81, -1, -1, -1);    // TCON 500KHz; Internal clock auto-enable
    else
        writeRegister(EPD_POWERCONTROL, 0xC1, -1, -1, -1);    // TCON 1MHz; Internal clock always enabled
    while (readRegister(0x15) == 0) {}                        // Wait until Internal Pump is ready 
}


// ************************************************************************************
// POWEROFF - Deactivates the high voltages needed to update the screen. The
// command should always be called after triggering an image update.
// ************************************************************************************
void PL_microEPD::powerOff() {
    writeRegister(EPD_POWERCONTROL, 0xD0, -1, -1, -1);
    waitForBusyInactive();
    writeRegister(EPD_POWERCONTROL, 0xC0, -1, -1, -1);
    waitForBusyInactive();
}


// ************************************************************************************
// Fills a segment of 24 x 24 px size with text, supporting textSize(<=4)
// ************************************************************************************
void PL_microEPD::printText(String text, int x, int y, int size){
    int i=0, xo=0, yo=0;
    x = _EPDwidth-x;                 // todo: Enable 12px @right column 
    y = _EPDheight-y-2;
    int ys = y, xs = x;
    setTextSize(size);
    clearBuffer();
       
    while (i<text.length()) {
        if (x<=_width){
          xo = _width - xs;
          x = _width;
        } else {
         //if (i!=0)
          clearBuffer();
         // memset(buffer, 0x55, sizeof(buffer)); 
         xo = 0;
        }    
        if (y<=_height) {
          yo = _height - ys;
          y = _height;
        } else
          yo = 0;
      
        setCursor(xo, yo);
        print(text.substring(i, i+1));
        writeRegister(EPD_WRITEPXRECTSET, y - _height, y -1, x - _width, x -1);
        writeRegister(EPD_PIXELACESSPOS, y - _height, x - _width, -1, -1);       
        writeBuffer(); 
        //if (x> _width)
          xs -= size*6;
          x -=size*6; 
        i += 1;
    }
}

void PL_microEPD::fillRectLM(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        clearBuffer();
        x = _EPDwidth-x;                 // todo: Enable 12px @right column 
        y = _EPDheight-y-2;
        fillRect(0, _height-h, w, _height, color);
        writeRegister(EPD_WRITEPXRECTSET, y - _height, y -1, x - _width, x -1);
        writeRegister(EPD_PIXELACESSPOS, y - _height, x - _width, -1, -1);       
        writeBuffer(); 
}

void PL_microEPD::drawBitmapLM(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h){
    clearBuffer();
    x = _EPDwidth-_width-x; 
    y = _EPDheight-_height-y;
    drawBitmap(0, 0, bitmap, w, h, 0); // WHITE=3 or BLACK=1
    writeRegister(EPD_WRITEPXRECTSET, y, y + _height-1, x, x + _width-1);
    writeRegister(EPD_PIXELACESSPOS, y, x, -1, -1); 
    writeBuffer();
}

// ************************************************************************************
// WRITEBUFFER - Sends the content of the memory buffer to the UC8156 driver IC.
// ************************************************************************************
void PL_microEPD::writeBuffer(boolean previousRAM){
    if (previousRAM)
        writeRegister(EPD_DATENTRYMODE, 0x37, -1, -1, -1);
    else 
        writeRegister(EPD_DATENTRYMODE, 0x27, -1, -1, -1);
    //SPI_Read_Array(cs, 0x10, buffer, _buffersize);      //(write)this clears the buffer..(?)
    
    digitalWrite(cs, LOW);
    SPI.transfer(0x10);
    for (int i=0; i < _buffersize; i++)
        SPI.transfer(buffer[i]);
    digitalWrite(cs, HIGH);
    
    waitForBusyInactive();
}


// ************************************************************************************
// WRITE REGISTER - Sets register ADDRESS to value VAL1 (optional: VAL2, VAL3, VAL4)
// ************************************************************************************
void PL_microEPD::writeRegister(uint8_t address, int16_t val1, int16_t val2, 
    int16_t val3, int16_t val4) {
    digitalWrite(cs, LOW);
    SPI.transfer(address);
    if (val1!=-1) SPI.transfer((byte)val1);
    if (val2!=-1) SPI.transfer((byte)val2);
    if (val3!=-1) SPI.transfer((byte)val3);
    if (val4!=-1) SPI.transfer((byte)val4);
    digitalWrite(cs, HIGH);
    waitForBusyInactive();
}

void PL_microEPD::writeRegisterNoWait(uint8_t address, int16_t val1, int16_t val2, 
    int16_t val3, int16_t val4) {
    digitalWrite(cs, LOW);
    SPI.transfer(address);
    if (val1!=-1) SPI.transfer((byte)val1);
    if (val2!=-1) SPI.transfer((byte)val2);
    if (val3!=-1) SPI.transfer((byte)val3);
    if (val4!=-1) SPI.transfer((byte)val4);
    digitalWrite(cs, HIGH);
    //waitForBusyInactive(EPD_TMG_SR2);
}
// ************************************************************************************
// READREGISTER - Returning the value of the register at the specified address
// ************************************************************************************
byte PL_microEPD::readRegister(char address){
    byte data;
    digitalWrite(cs, LOW);                                      
    SPI.transfer(address | EPD_REGREAD);
    data = SPI.transfer(0xFF);                         
    digitalWrite(cs, HIGH);
    waitForBusyInactive();
    return data;                                        // can be improved
}


// ************************************************************************************
// WHITE ERASE - Triggers two white updates to erase the screen and set back previous
// ghosting. Recommended after each power cycling.
// ************************************************************************************
void PL_microEPD::WhiteErase() {
  clear(EPD_WHITE);
  update();
}


// ************************************************************************************
// WAITFORBUSYINACTIVE - Sensing to ‘Busy’ pin to detect the UC8156 driver status.
// Function returns only after driver IC is free again for listening to new commands.
// ************************************************************************************
void PL_microEPD::waitForBusyInactive(){
    while (digitalRead(busy) == LOW) {}
}

// ************************************************************************************
// DEEPSLEEP - Putting the UC8156 in deep sleep mode with less than 1µA current @3.3V.
// Reset pin toggling needed to wakeup the driver IC again.
// ************************************************************************************
void PL_microEPD::deepSleep(void) {
    writeRegister(0x21, 0xff, 0xff, 0xff, 0xff); 
}

void PL_microEPD::end(void) {
    digitalWrite(rst, HIGH);  // Turn VDD for EPD off and put remaining 
    delay(5);
    digitalWrite(9, LOW);    // GPIOs LOW to minimize power consumption 
    delay(5);
    digitalWrite(rst, LOW);   // to around 1µA
    pinMode(busy, OUTPUT);
    digitalWrite(busy, LOW);
    digitalWrite(cs, LOW);
}
