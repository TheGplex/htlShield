/******************************************************************
                            o l e d 
                                                    қuran nov 2022
******************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "init.h"
#include "oled.h"

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void initOled(void)
{
    oled.begin(SSD1306_SWITCHCAPVCC, OLED);
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(0, 0);
    oled.print("start!");
    // oled.drawRect(10, 25, 40, 15, WHITE); // links, unten, breit, hoch
    // oled.drawLine(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    // oled.drawCircle(64, 32, 31, WHITE);
    oled.display();
}

void oledClrDisplay(void)
{
    oled.clearDisplay();
}
              
void oledPrintfxy(int x, int y, char * t)
{
    oled.setCursor(x, y);
    oled.print(t);
}

void oledRefresh(void)
{ 
    oled.display();
}
              
                  


