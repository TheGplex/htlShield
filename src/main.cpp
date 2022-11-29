/******************************************************************
                        Simple State Machine
                                                    қuran nov 2022
******************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "init.h"
#include "oled.h"
#include "mfc.h"
#include "timer.h"
#include "adc.h"


#define STATE_GREEN    0
#define STATE_BLUE     1
#define STATE_RED      2

// global variables:

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
volatile int tick, tim, tim2, timADC, flag1, flag2, dialControl, flagd;
volatile unsigned char red, green, blue;



int  getPoti(void);
int  getLDR(void);

void setMulticolorLed(unsigned char r, unsigned char g, unsigned char b);

void setLeds(int x);


void setup()
{
    initHtlShield();



    cli();
    TCCR1A = 0;              // Normalmode  16 Bit Timerblock
    TCCR1B = 1 << CS10;      // No prescaler
    TIMSK1 = 1 << TOIE1;
    flag1 = flag2 = flagd = FALSE;
    tick = 0;
    dialControl = 0;
    sei();                   // enable all interrupts

    Serial.begin(115200);
    Wire.begin();

    Serial.println("start!");

    oled.begin(SSD1306_SWITCHCAPVCC, OLED);
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(0, 0);
    oled.print("htlShield");
    // oled.drawRect(10, 25, 40, 15, WHITE); // links, unten, breit, hoch
    // oled.drawLine(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    // oled.drawCircle(64, 32, 31, WHITE);
    oled.display();
                             // open communication with HMC5883
    Wire.beginTransmission(MFS);
    Wire.write(0x02);        // select mode register
    Wire.write(0x00);        // continuous measurement mode
    Wire.endTransmission();
}

void loop()
{
    static int state = STATE_GREEN;

    int x, y, z, akn;
    char text[TXT_LENGTH];


    if (flagd == TRUE)
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

              Wire.beginTransmission(MFS);
              Wire.write(0x03);
              akn = Wire.endTransmission();

              Wire.requestFrom(MFS, 6);
              if (6 <= Wire.available())
              {
                  x =  Wire.read() << 8; x |= Wire.read();
                  z =  Wire.read() << 8; z |= Wire.read();
                  y =  Wire.read() << 8; y |= Wire.read();
              }

              sprintf(text, "akn: %d x: %d y %d z: %d", akn, x, y, z);
              Serial.println(text);

              oled.clearDisplay();
              
              sprintf(text, "x: %d", x);
              oled.setCursor(0, 0);
              oled.print(text);
              
              /* 
              oled.setCursor(0, 16);
              x = analogRead(A0);
              sprintf(text, "a0: %d", x);
              oled.print(text);


              oled.setCursor(64, 16);
              x = analogRead(A1);
              sprintf(text, "a1: %d", x);
              oled.print(text);
*/
              
              oled.setCursor(0, 32);
              x = dialControl;
              sprintf(text, "d: %d", x);
              oled.print(text);

              
              oled.display();

               // angleXY = atan2(-y,  x) / M_PI * 180;  if (angleXY < 0) angleXY += 360;
               // angleYZ = atan2(-z, -y) / M_PI * 180;  if (angleYZ < 0) angleYZ += 360;
               // angleZX = atan2( x, -z) / M_PI * 180;  if (angleZX < 0) angleZX += 360;
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


void setMulticolorLed(unsigned char r, unsigned char g, unsigned char b)
{
    red = r;
    green = g;
    blue = b;
}

void setLeds(int x)
{
    Wire.beginTransmission(PCF8574);
    Wire.write(~x); 
    Wire.endTransmission();
}

