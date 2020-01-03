// PCD8544 driver
// 
// (c) Peter Niebert, Aix-Marseille Université, 2018
// inspired by NOKIA_5110 of Chris Yan 2012-2014
// see 
//  Desc: Commands, fonts, and class for using a
//      Nokia 5110 LCD via the Phillips 8554 LCD driver.
// 
//  Typical Usage: User must fill the LcdPins struct with the pinout used to control the LCD and
//      instantiate the NokiaLcd class - passing the created LcdPins struct to the constructor.
//      The class function NokiaLcd::InitLcd may then be called to reset and start the LCD driver.
//      A simple 6x6 font (6x8 in LCD space and ~5x5 character space) is included to facilitate 
//      the NokiaLcd::DrawChar( char character ) function, which will copy the character 8 bits 
//      at a time for 6 clock cycles.
//                 Commands may be sent to the LCD via the NokiaLcd::SendFunction(char cmd) 
//      function, but be aware that certain commands require the Function Set register's H-value
//      to be either 1 or 0, depending on the command. This class does not check to see whether
//      the H-value is of proper status. The Function Set register /may/ be changed via the 
//      NokiaLcd::SendFunction(char cmd), but the code uses this internally and expects that
//      most function registers have not been changed by the user.
//
//      Example:
//          #include "mbed.h"
//          #include "NOKIA_5110.h"
//
//          int main() {
//              LcdPins myLcdPins = { p11, NC, p13, p10, p8, p9 };
//              NokiaLcd myLcd( myLcdPins );    // SPI is started here (8-bits, mode 1)
//              myLcd.InitLcd();                // LCD is reset and DDRAM is cleared
//              myLcd.TestLcd( 0xAA );          // Draws a vertical pattern where every other pixel is on 
//              wait(10);                       
//              myLcd.ShutdownLcd();            // Clears the LCD's DDRAM and powers it down via CMD_FS_POWER_DOWN_MODE, H=0
//              while(1)
//              {   };
//          }

// Command Instructions
//       H = 0
#ifndef __PCD8544_H__
#define __PCD8544_H__


#include "mbed.h"


struct LcdFunctionSet
{
    char PD;
    char V;
    char H;
};


class PCD8544
{
    public:
        PCD8544(PinName mosi, PinName sclk,PinName dc, PinName sce,PinName rst,PinName power, PinName led);
        ~PCD8544();
        
    public:
        void activate();
        void shutdown();
        void clear();
        void drawString(const char *str);
        //void drawChar(char character);
        void setXY(char x, char y);
        void setBacklight(bool on);
        
    private:
        char createFunctionChar();
        void reset();
        void sendFunction(char cmd);
        void sendDrawData(char data);


        
    private:
        char functionChar;
        char  tempControlChar;
        char  dispControlChar;
        LcdFunctionSet  functionSet;
        char     biasChar;
        char      vopChar;
        SPI spi;

        DigitalOut pin_rst;
        DigitalOut pin_sce;
        DigitalOut pin_dc;
        DigitalOut pin_power;
        DigitalOut pin_backlight;




        
};


#endif
