/******************************************************************
    t i m e r   i n t e r r u p t , k e y s , t i m , p w m 
                                                    қuran nov 2022
******************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "init.h"
#include "timer.h"


volatile int tick, tim0, tim1, timADC, flag1, flag2, dialControl, flagd, x, led;
extern volatile unsigned char red, green, blue;

int getPeakLeds(void)
{
    return led;
}

int getxPeak(void)
{
    return x; 
}

int keyPressed(int key)
{
    int ret = 0; 

    switch (key)
    {
        case KEY_1:    ret = (flag1 == TRUE)? TRUE : FALSE; break; 
        case KEY_2:    ret = (flag2 == TRUE)? TRUE : FALSE; break; 
        case DIAL_KEY: ret = (flagd == TRUE)? TRUE : FALSE; break; 
    }
    return ret;
}
void clearKey(int key)
{
    switch (key)
    {
        case KEY_1:    flag1 = FALSE; break;
        case KEY_2:    flag2 = FALSE; break;
        case DIAL_KEY: flagd = FALSE; break;
    }
}
void resetDialControl(void)
{
    dialControl = 0; 
}

int getDialControl(void)
{
    return dialControl;
}

int getDialControlCenter(void)
{
    int y, x; 

    y = getDialControl(); 


    if (y >   4)      x = 0xff;
    else if (y ==  4) x = 0x80;
    else if (y ==  3) x = 0x40;
    else if (y ==  2) x = 0x20;
    else if (y ==  1) x = 0x10;
    else if (y ==  0)  x = 0x08;
    else if (y == -1) x = 0x04;
    else if (y == -2) x = 0x02;
    else if (y == -3) x = 0x01;
    else              x = 0x0;

    return x;


}

void startTimer(int t, int time)
{
    switch (t)
    {
        case TIMER_0: tim0 = time; break; 
        case TIMER_1: tim1 = time; break;
    }
}


int timerexpired(int t)
{
    int ret = 0; 
    switch(t)
    {
        case TIMER_0: ret = (tim0 == 0) ? TRUE : FALSE;  break;
        case TIMER_1: ret = (tim1 == 0) ? TRUE : FALSE;  break;
    }
    return ret; 
}




void initTimer(void)
{
    cli();
    TCCR1A = 0;              // Normalmode  16 Bit Timerblock
    TCCR1B = 1 << CS10;      // No prescaler
    TIMSK1 = 1 << TOIE1;
    flag1 = flag2 = flagd = FALSE;
    tick = 0;
    dialControl = 0;
    sei();                   // enable all interrupts
}

ISR (TIMER1_OVF_vect)
{
    static unsigned char ramp = 0;
    static int key1 = RELEASED, oldkey1 = RELEASED;
    static int key2 = RELEASED, oldkey2 = RELEASED;
    static int dial = RELEASED, oldDial = RELEASED;
    static int keyd = RELEASED, oldkeyd = RELEASED;
    static int array[8] = {0};
    static int j;
    static int oldx = 0, star = 0;

    TCNT1 = 65536 - 1600;    // 1 step takes 62.5 nsec -> 16 = 1 µsec; 1600 = 0.1 msec
                             // such settings should stay in the first line,
                             // of the interrupt: always!

    ramp++;

    if (red   > ramp) PORTB |=  LED_RED;   else PORTB &= ~LED_RED;
    if (green > ramp) PORTB |=  LED_GREEN; else PORTB &= ~LED_GREEN;
    if (blue  > ramp) PORTD |=  LED_BLUE;  else PORTD &= ~LED_BLUE;


    if ((tick % 10) == 0)    // all 1msec
    {
        dial = ((PINB & D2) == D2) ? RELEASED : PRESSED;
        if ((dial == PRESSED) && (oldDial == RELEASED))
        { 
            if ((PIND & D1) == D1) dialControl++; else dialControl--;
        } 
        if ((dial == RELEASED) && (oldDial == PRESSED))    
        { 
            if ((PIND & D1) == 0)  dialControl++; else dialControl--;
        } 
        oldDial = dial;

        if (tim0   > 0) tim0--;             // to measure time
        if (tim1   > 0) tim1--;            // to measure time
        if (timADC > 0) timADC--;          // to measure time
    }



    if (tick >= 100)          // all 10 msec
    {
        tick = 0;

        key1 = ((PIND & T1) == T1)   ? RELEASED : PRESSED;
        key2 = ((PIND & T2) == T2)   ? RELEASED : PRESSED;
        keyd = ((PIND & DT) == DT)   ? RELEASED : PRESSED;

        if ((key1 == PRESSED) && (oldkey1 == RELEASED)) { flag1 = TRUE; } oldkey1 = key1;
        if ((key2 == PRESSED) && (oldkey2 == RELEASED)) { flag2 = TRUE; } oldkey2 = key2;
        if ((keyd == PRESSED) && (oldkeyd == RELEASED)) { flagd = TRUE; } oldkeyd = keyd;

    

        // Peakhold: ebenfalls alle 10 msec

        x = ADCH;
        x =((x>>4) + 1) >> 1;
        for (j = 0; j < x; j++ ) array[j] = 1;  // 0 bis 7 ! 

        led = 0;

        for (j = 0; j < 8; j++)
        {
            if (array[j] > 0) 
            {
                led |= (1 << j);
                array[j]--;
            }
        }

        if (x < oldx)    
        {
            array[star] = 100;
            star = 0;
        }
        else if (x > oldx) star = x - 1;
        
        oldx = x;
    }
    tick++;


}



