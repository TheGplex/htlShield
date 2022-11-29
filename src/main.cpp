/******************************************************************
                        Simple State Machine
                                                    қuran nov 2022
******************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "init.h"
#include "adc.h"
#include "leds.h"
#include "oled.h"
#include "mfc.h"
#include "timer.h"


// global variables:


volatile int tick, tim, tim2, timADC, flag1, flag2, dialControl, flagd;
volatile unsigned char red, green, blue;

#define STATE_GREEN    0
#define STATE_BLUE     1
#define STATE_RED      2

#define STATE_0        0 
#define STATE_1        1
#define STATE_2        2
#define STATE_3        3   

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    
    initHtlShield();
    initTimer(); 
    initOled();
    initMFC();

    Serial.println("start!");

}

void loop()
{
    static int state = STATE_GREEN;
    int x, y, z, akn;
    char text[TXT_LENGTH];

    if (flagd == TRUE)    // in allen states soll es möglich sein, zu resitieren
    {
        flagd = FALSE;
        dialControl = 0; 
    }



    

    switch (state)
    {
        case STATE_GREEN: setMulticolorLed(0, 10, 0);

            x = getPoti();

            setLeds(x);


            if (flag1 == TRUE)
            {
                state = STATE_BLUE;
                setMulticolorLed(0, 0, 128);  // blue

                flag1 = FALSE;
                flag2 = FALSE;
            }

        break;

        case STATE_BLUE:  

            setMulticolorLed(0, 0, 128);


            if (flag1 == TRUE)
            {
                state = STATE_RED;
                tim = FIVE_SEC;
            }

            if (flag2 == TRUE)
            {
                state = STATE_GREEN;
                flag1 = FALSE;
            }

            if (tim2 == 0)
            {
                tim2 = ONE_SEC;

                akn = getMFC(&x, &y, &z);


              sprintf(text, "akn: %d x: %d y %d z: %d", akn, x, y, z);
              Serial.println(text);

              oledClrDisplay();
              
              sprintf(text, "x: %d", x);
              oledPrintfxy(0, 0, text);
                            
              x = dialControl;
              sprintf(text, "d: %d", x);
              oledPrintfxy(0, 32, text);
              oledRefresh(); 

            }


        break;

        case STATE_RED:  // setMulticolorLed(128, 0, 0);

          if (tim == 0)
          {
             state = STATE_GREEN;
             flag1 = FALSE;
          }

          x = getLDR();

          setMulticolorLed(x, 0, 0);

        break;

    }

}



