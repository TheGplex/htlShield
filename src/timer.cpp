/******************************************************************
                  t i m e r   i n t e r r u p t
                                                    қuran nov 2022
******************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "init.h"
#include "timer.h"

extern volatile int tick, tim, tim2, timADC, flag1, flag2, dialControl, flagd;
extern volatile unsigned char red, green, blue;


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

        if (tim    > 0) tim--;             // to measure time
        if (tim2   > 0) tim2--;            // to measure time
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

    }
    tick++;
}


