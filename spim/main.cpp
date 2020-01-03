// ------------ SPIM ------------ //
// ----------- MASTER ----------- //
// --- PROJET CASES SOLIVERES --- //
// --------------  -------------- //

#include <mbed.h>

DigitalOut led(LED1);
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK);
DigitalOut ss(SPI_MS1, 1);

uint32_t txbuf[20];
uint32_t rxbuf[20];

// Ajout capteur T°
void setup_temp()
{
    NRF_FICR_Type *f = NRF_FICR;
    NRF_TEMP_Type *t = NRF_TEMP;
    t->A0 = (f->TEMP).A0;
    t->A1 = (f->TEMP).A1;
    t->A2 = (f->TEMP).A2;
    t->A3 = (f->TEMP).A3;
    t->A4 = (f->TEMP).A4;
    t->A5 = (f->TEMP).A5;
    t->B0 = (f->TEMP).B0;
    t->B1 = (f->TEMP).B1;
    t->B2 = (f->TEMP).B2;
    t->B3 = (f->TEMP).B3;
    t->B4 = (f->TEMP).B4;
    t->B5 = (f->TEMP).B5;
    NRF_TEMP->TASKS_START = 1;
}

float temp_read()
{
    // return 25.25;
    int raw_temp = NRF_TEMP->TEMP;
    float temp = raw_temp * 0.25;
    NRF_TEMP->TASKS_START = 1;
    return temp;
}
// fin du capteur T°

void done(int v)
{
    ss = 1;
    led.write(rxbuf[0] != 0);
}

volatile int i = 0;

int main()
{
    spi.format(8, 3);       // 8 bits par transfert, 3 = polarité de l'horloge
    spi.frequency(1000000); // MegaHertz (10^6)
    int lotOfSec = 0;       // pour le compteur d'uPtime

    while (true)
    {
        txbuf[0] = i;
        txbuf[1] = lotOfSec;

        i = temp_read();
        lotOfSec ++; 

        ss = 0;
        spi.transfer(reinterpret_cast<uint8_t *>(txbuf), 8, reinterpret_cast<uint8_t *>(rxbuf), 8, done);
        ThisThread::sleep_for(1000);
    }
}
