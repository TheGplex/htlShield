/******************************************************************
                            l e d s
                                                    қuran nov 2022
******************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "init.h"
#include "leds.h"

//extern volatile 
volatile unsigned char red, green, blue;

void setLeds(int x)
{
    Wire.beginTransmission(PCF8574);
    Wire.write(~x); 
    Wire.endTransmission();
}

void setMulticolorLed(unsigned char r, unsigned char g, unsigned char b)
{
    red = r;
    green = g;
    blue = b;
}

