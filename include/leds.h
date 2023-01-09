/******************************************************************
                            l e d s
                                                    қuran nov 2022
******************************************************************/

/// @brief this function controlls the smd-leds
/// @note this function uses wire.h!
/// @param x each of the first 8 bits stands for a own led. 
void setLeds(int x);



/// @brief 
/// @param r Rot-Anteil 
/// @param g Grün-Anteil
/// @param b Blau-Anteil
void setMulticolorLed(unsigned char r, unsigned char g, unsigned char b);
// what for: diese Funktion steuert die Multi-Color-Led
// PRE: nothing
// POST: die drei Farben der Multicolor Led sind eingestellt
// RETURN: nothing
