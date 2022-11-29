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

int getLDR(void)
{
    int x;
    if ((ADMUX & (1 << MUX0)) == 0)           { timADC = 3; while (timADC); } 
    ADMUX |= (1 << MUX0);
    return x =  ADCH;
}
