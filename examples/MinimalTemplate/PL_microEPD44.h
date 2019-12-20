/* *****************************************************************************************
PL_MICROEPD - A Universal Hardware Library for 1.1”, 1.4", 2.1" and 3.1" E-Paper displays 
(EPDs) from Plastic Logic based on UC8156 driver IC. The communication is SPI-based, for more
information about hook-up and tutorials please check: https://github.com/RobPo/Paperino.

Created by Robert Poser, Feb 9th 2018, Dresden/Germany. Released under BSD license
(3-clause BSD license), check license.md for more information.

We invested time and resources providing this open source code, please support Paperino and
open source hardware by purchasing this product @Crowd_supply @Watterott @Plasticlogic
***************************************************************************************** */
#ifndef PL_microEPD_h
#define PL_microEPD_h

#include "Adafruit_GFX.h"               

#define EPD_WIDTH   (146)
#define EPD_HEIGHT  (240)

#define EPD_BLACK 0x00
#define EPD_DGRAY 0x01
#define EPD_LGRAY 0x02
#define EPD_WHITE 0x03

#define EPD_UPD_FULL  0x00            // Triggers a Full update, 4 GL, 800ms
#define EPD_UPD_PART  0x01            // Triggers a Partial update, 4 GL, 800ms
#define EPD_UPD_MONO  0x02            // Triggers a Partial Mono update, 2 GL, 250ms
#define EPD_UPD_LOWP  0x03            // Triggers a Partial Mono update, 2 GL, 250ms, low power

#define EPD_REVISION          0x00  // Revision, Read only
#define EPD_PANELSETTING      0x01
#define EPD_DRIVERVOLTAGE     0x02
#define EPD_POWERCONTROL      0x03
#define EPD_BOOSTSETTING      0x04 
#define EPD_INTERVALSETTING   0x05 
#define EPD_TCOMTIMING        0x06
#define EPD_INTTEMPERATURE    0x07
#define EPD_SETRESOLUTION     0x0C
#define EPD_WRITEPXRECTSET    0x0D
#define EPD_PIXELACESSPOS     0x0E
#define EPD_DATENTRYMODE      0x0F
#define EPD_BYPASSRAM         0x12
#define EPD_DISPLAYENGINE     0x14
#define EPD_VCOMCONFIG        0x18
#define EPD_BORDERSETTING     0x1D
#define EPD_POWERSEQUENCE     0x1F
#define EPD_SOFTWARERESET     0x20
#define EPD_PROGRAMMTP        0x40
#define EPD_MTPADDRESSSETTING 0x41
#define EPD_LOADMONOWF        0x44
#define EPD_REGREAD           0x80  // Instruction R/W bit set HIGH for data READ

class PL_microEPD : public Adafruit_GFX {

public:
    PL_microEPD(int8_t _cs, int8_t _rst=-1, int8_t _busy=-1);

    void begin(bool erase=true);
    void clearBuffer();
    void clear(int c=EPD_WHITE);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void update(int updateMode=EPD_UPD_FULL);
    void setVBorderColor(int color);
    void powerOn(bool lowpower=0);
    void powerOff(void);
    void writeRegister(uint8_t address, int16_t val1, int16_t val2, int16_t val3, int16_t val4);
    void writeBuffer(boolean previousRAM = false);
    void printText(String text, int x, int y, int s);
    void setCursorSegment(int x, int y);
    void drawBitmapLM(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h);
    void fillRectLM(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    byte buffer[144];           //buffer for 24x24 character (size 2)
    void waitForBusyInactive();
    void deepSleep(void);
    void end(void);

private:
    int _EPDsize, _EPDwidth, _EPDheight, _buffersize;
    int cs, rst, busy;
    int nextline;
    void writeRegisterNoWait(uint8_t address, int16_t val1, int16_t val2, int16_t val3, int16_t val4);
    byte readRegister(char address);
    void scrambleBuffer(void);
    //void writeBuffer(boolean previousRAM = false);
    void WhiteErase(void);

};

#endif
