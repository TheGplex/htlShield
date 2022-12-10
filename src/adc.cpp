/******************************************************************
        a n a l o g   d i g i t a l   c o n v e r s i o n 
                                                    қuran nov 2022
******************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "init.h"
#include "adc.h"

extern volatile int timADC;

int getPoti(void)
{
    int x;
    if ((ADMUX & (1 << MUX0)) == (1 << MUX0)) { timADC = 3; while (timADC); } 
    ADMUX &= ~(1 << MUX0);
    return x =  ADCH;
}

int getPotiLin(void)
{
    int x, y;

    y = getPoti(); 

    if (y > 240)      x = 0xff;
    else if (y > 210) x = 0x7f;
    else if (y > 180) x = 0x3f;
    else if (y > 150) x = 0x1f;
    else if (y > 120) x = 0x0f;
    else if (y >  90) x = 0x07;
    else if (y >  60) x = 0x03;
    else if (y >  30) x = 0x01;
    else              x = 0x0;

    return x;
}



int getLDR(void)
{
    int x;
    if ((ADMUX & (1 << MUX0)) == 0)           { timADC = 3; while (timADC); } 
    ADMUX |= (1 << MUX0);
    return x =  ADCH;
}
