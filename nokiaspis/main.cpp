#include "mbed.h"
#include <mbed_events.h>

#include "PCD8544.h"
#include "NRFSPIS.h"

DigitalOut led(LED1);
PCD8544 lcd(P0_3, P0_2, P0_4, P0_26, P0_12, P0_27, P0_7);

InterruptIn button(BUTTON1);

void received_data();

NRFSPIS spis(SPIS_MOSI, SPIS_MISO, SPIS_SCK, SPIS_SS, NRF_SPIS_MODE_3, 1);

EventQueue queue;

volatile uint32_t buttonState(0);

int32_t txbuffer[100];
int32_t rxbuffer[100];

void buttonUp()
{
    txbuffer[0] = 0;
}

void buttonDown()
{
    txbuffer[0] = 1;
}

void showdata(int dataNbr, int LineArray);

void ledoff()
{
    led = 0;
    showdata(0, 0);
}

void received_data()
{
    led = 1;
    queue.call_in(1, ledoff);
}

void showdata(int dataNbr, int lineArray)
{
    int32_t data = rxbuffer[dataNbr];
    char string[20];
    lcd.setXY(40, lineArray);
    sprintf(string, "       ");
    lcd.drawString(string);
    lcd.setXY(40, lineArray);
    sprintf(string, "%6d", (int)data);
    lcd.drawString(string);
    //txbuffer[0] = buttonState;

    spis.transfer(reinterpret_cast<uint8_t *>(txbuffer), 4, reinterpret_cast<uint8_t *>(rxbuffer), 100, received_data);
}


/*
    int32_t data2=rxbuffer[4];
    //char string[20];
    lcd.setXY(40,0);
    sprintf(string,"%6d",(int)data2);
    lcd.drawString(string);

    // -------cutTime display
    //lotOfSec
    int32_t data2=rxbuffer[2];
    lcd.setXY(40,0);
    sprintf(string,"%6d",(int)data2);
    lcd.drawString(string);
    //hours
    int32_t data4=rxbuffer[4];
    lcd.setXY(40,0);
    sprintf(string,"%6d",(int)data4);
    lcd.drawString(string);
    //minutes
    int32_t data6=rxbuffer[6];
    lcd.setXY(40,0);
    sprintf(string,"%6d",(int)data6);
    lcd.drawString(string);
    //seconds
    int32_t data8=rxbuffer[8];
    lcd.setXY(40,0);
    sprintf(string,"%6d",(int)data8);
    lcd.drawString(string);
    // --------end cutTime display
    

    
    spis.transfer(reinterpret_cast<uint8_t *>(txbuffer), 4, reinterpret_cast<uint8_t *>(rxbuffer), 100, received_data);
*/

int main()
{

    button.rise(buttonUp);
    button.fall(buttonDown); //

    char string[20];

    ThisThread::sleep_for(100);

    lcd.activate();
    sprintf(string, "Temp: ");
    lcd.drawString(string);
    rxbuffer[0] = 42;
    showdata(0,0);
    lcd.setXY(0,2);
    sprintf(string, "Time: ");
    lcd.drawString(string);
    showdata(4,2);

    queue.dispatch_forever();
}
