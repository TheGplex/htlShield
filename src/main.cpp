/******************************************************************
                        Simple State Machine
                                                    қuran nov 2022
******************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define TXT_LENGTH   100
#define PCF8574     0x20     // i2c address
#define OLED        0x3c
#define MFS         0x1e     // magnetic field sensor
#define LED         0x20     // arduino on board led

#define LED_RED     0x04     // PORTB
#define LED_GREEN   0x02     // PORTB
#define D2          0x01     // PORTB Dial Control 2

#define T1          0x80     // PORTD
#define T2          0x40     // PORTD
#define LED_BLUE    0x20     // PORTD
#define D1          0x08     // PORTD Dial Control 1
#define DT          0x04     // PORTD Dial Control Taster

#define STATE_GREEN    0
#define STATE_BLUE     1
#define STATE_RED      2

#define ONE_SEC     1000     // 1000 * 1 msec = 1 second
#define FIVE_SEC    5000     // 5000 * 1 msec = 5 seconds
#define TRUE           1
#define FALSE          0
#define PRESSED        0
#define RELEASED       1

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
volatile int tick, tim, tim2, flag1, flag2, dialControl, flagd;

int  getLevel(void);
void setMulticolorLedGreen(void);
void setMulticolorLedBlue(void);
void setMulticolorLedRed(void);

void setup()
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

    tick = 0;

    ADMUX = (1<<REFS0) | (1<<ADLAR);   // Poti A0
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADPS2);

    cli();
    TCCR1A = 0;              // Normalmode  16 Bit Timerblock
    TCCR1B = 1 << CS10;      // No prescaler
    TIMSK1 = 1 << TOIE1;
    flag1 = flag2 = flagd = FALSE;
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

    x = getLevel();

    Wire.beginTransmission(PCF8574);
    Wire.write(x >> 2); 
    Wire.endTransmission();

    if (flagd == TRUE)
    {
        flagd = FALSE;
        dialControl = 0; 
    }

    switch (state)
    {
      case STATE_GREEN: setMulticolorLedGreen();

          if (flag1 == TRUE)
          {
             state = STATE_BLUE;
             setMulticolorLedBlue();

             flag1 = FALSE;
             flag2 = FALSE;
          }

      break;

      case STATE_BLUE:  

          setMulticolorLedBlue();


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

      case STATE_RED:  setMulticolorLedRed();

          if (tim == 0)
          {
             state = STATE_GREEN;
             flag1 = FALSE;
          }

      break;

    }

}

ISR (TIMER1_OVF_vect)
{
    static int key1 = RELEASED, oldkey1 = RELEASED;
    static int key2 = RELEASED, oldkey2 = RELEASED;
    static int dial = RELEASED, oldDial = RELEASED;
    static int keyd = RELEASED, oldkeyd = RELEASED;

    TCNT1 = 65536 - 16000;   // 1 step takes 62.5 nsec -> 16 = 1 µsec; 16000 = 1 msec
                             // such settings should stay in the first line,
                             // of the interrupt: always!

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

    if (tim  > 0) tim--;  // to measure time
    if (tim2 > 0) tim2--; // to measure time


    if (tick >= 10)          // 10 msec
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

int getLevel(void)
{
    int x;
    return x =  (ADCH << 8) + ADCL;
}

void setMulticolorLedGreen(void){PORTB |=  LED_GREEN; PORTD &= ~LED_BLUE; PORTB &= ~LED_RED;}
void setMulticolorLedBlue (void){PORTB &= ~LED_GREEN; PORTD |=  LED_BLUE; PORTB &= ~LED_RED;}
void setMulticolorLedRed  (void){PORTB &= ~LED_GREEN; PORTD &= ~LED_BLUE; PORTB |=  LED_RED;}