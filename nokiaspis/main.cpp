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

int dataMode = 0;     //mode selon appuie btn
const int nbMode = 5; //constant qui déf le nombre de modes

void buttonUp()
{
    txbuffer[0] = 0;
}

void buttonDown()
{
    txbuffer[0] = 1;
    dataMode++;
    dataMode %= nbMode; // pour que 0<=dataMode<nbMode
}

void display(int x, int y, const char *string);
void showdata();

void ledoff()
{
    led = 0;
    showdata();
}

void received_data()
{
    led = 1;
    queue.call_in(1, ledoff);
}

// ajout compteur de temps
unsigned int lotOfSec = 0;

typedef struct temps
{
    unsigned int hour;
    unsigned int minute;
    unsigned int second;
} temps_t;

temps_t cutTime(unsigned int);

temps_t cutTime(unsigned int lotOfSec)
{
    temps_t hoMiSe; //hourMInuteSEcond..
    hoMiSe.hour = lotOfSec / 3600;
    lotOfSec %= 3600;
    hoMiSe.minute = lotOfSec / 60;
    lotOfSec = lotOfSec % 60;
    hoMiSe.second = lotOfSec;

    return hoMiSe;
}
// fin compteur temps

temps_t result;

void showdata()
{
    char txt2Display[30];
    lcd.clear();

    uint32_t temperature = rxbuffer[0];
    uint32_t lotOfSec = rxbuffer[1];

    switch (dataMode)
    {
    case 0:
        sprintf(txt2Display, "Temp : %u", temperature);
        display(0, 0, txt2Display);
        break;

    case 1:
        display(26, 1, "UpTime");
        sprintf(txt2Display, "On Since %us", lotOfSec);
        display(0, 2, txt2Display);
        break;

    case 2:
        result = cutTime(lotOfSec);
        sprintf(txt2Display, "h:%02u m:%02u s:%02u", result.hour, result.minute, result.second);
        display(6, 1, "UpTime h/m/s");
        display(0, 2, txt2Display);
        break;

    case 3:
        sprintf(txt2Display, "Temp : %u", temperature);
        display(0, 0, txt2Display);
        
        display(26, 2, "UpTime");
        sprintf(txt2Display, "On Since %us", lotOfSec);
        display(0, 3, txt2Display);
        
        result = cutTime(lotOfSec);
        sprintf(txt2Display, "h:%02u m:%02u s:%02u", result.hour, result.minute, result.second);
        display(0, 5, txt2Display);
        break;

    case 4:
        sprintf(txt2Display, "u32:%d", sizeof(unsigned int));
        display( 0, 4, txt2Display);
        break;

    default:
        break;
    }

    spis.transfer(reinterpret_cast<uint8_t *>(txbuffer), 4,
                  reinterpret_cast<uint8_t *>(rxbuffer), 8, received_data);
}

void display(int x, int y, const char *string)
{
    lcd.setXY(x, y);
    lcd.drawString(string);
}

int main()
{

    button.rise(buttonUp);
    button.fall(buttonDown); //

    ThisThread::sleep_for(100);

    lcd.activate();
    rxbuffer[0] = 42;
    showdata();

    queue.dispatch_forever();
}
