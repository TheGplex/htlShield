/******************************************************************
                    analog digital conversion
                                                    қuran nov 2022
******************************************************************/


/// @brief Ermittelt den Analogwert des Potis 
/// @param - 
/// @return Wertebereich 0 bis 255 (Poti)
int  getPoti(void);
//   RET: 0 up to 255 = the Poti position

int  getPotiLin(void);
//   RET: 0 up to 0xff = poti position in this form:
//   *         = 0x01
//   **        = 0x03
//   ***       = 0x07
//   ****      = 0x0f ....  and so far. 

int  getLDR(void);
//   RET: 0 up to 255  = measures the brightness
