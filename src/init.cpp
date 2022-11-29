/******************************************************************
                        all inits
                                                    қuran nov 2022
******************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "init.h"

void initHtlShield(void)
{
    DDRB = LED_RED | LED_GREEN | LED;
    DDRB &= ~D2;

    DDRD = LED_BLUE;
    DDRD &= ~T1;
    DDRD &= ~T2;
    DDRD &= ~D1;
    DDRD &= ~DT;

    PORTB = PORTB | D2;                // Pullups!
    PORTD = PORTD | T1 | T2 | D1 | DT; // Pullups!
    
    ADMUX = (1<<REFS0) | (1<<ADLAR);   // Poti A0
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADPS2);

}
    
