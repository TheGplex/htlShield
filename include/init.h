/******************************************************************
                        all inits
                                                    қuran nov 2022
******************************************************************/

#ifndef INIT
#define INIT

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

#define ONE_SEC     1000     // 1000 * 1 msec = 1 second
#define FIVE_SEC    5000     // 5000 * 1 msec = 5 seconds
#define TIMER_0        0
#define TIMER_1        1


#define TRUE           1
#define FALSE          0
#define PRESSED        0
#define RELEASED       1

#define KEY_1       0x01
#define KEY_2       0x02
#define DIAL_KEY    0x40



void initHtlShield(void);
// initializes the hardware of the shield. should be called during the setup

#endif