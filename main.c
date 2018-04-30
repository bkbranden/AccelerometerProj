#include <msp430.h> 
#include <math.h>
#include "cordic.h"
#include "pushbutton.h"
#include "timerA.h"
#include "spi.h"
#include "ADC.h"
#include "debounce.h"
#include "LED_display.h"

#define CORDIC_GAIN 0.607252935103139
#define CORDIC_GAIN_X_EQUALS_Y (float) (sqrt(2)/2)
#define PI 3.1415926536897932384626

// Function Prototypes
void ConfigureClockModule();
void CalculateAve(void);
void CalibrateACC(void);
void CalculateOrigins(void);
void CenterXYZ(void);
void CalculateAngleHypPhi(void);
void ManageSoftwareTimers(void);
void dispLED(void);

// Global variables
unsigned int g1msTimer = 0;
unsigned int g1msTimeout = 0;
unsigned int calibrationCount = 0;
unsigned int X_Axis_Sum, Y_Axis_Sum, Z_Axis_Sum;
signed int X_Ave, Y_Ave, Z_Ave;
unsigned int Xmax, Xmin, Ymax, Ymin, Zmax, Zmin;
unsigned int Xorigin, Yorigin, Zorigin;
volatile float angle, hypotenuse, phi;
unsigned int counterA =0;
unsigned int counterB = 0;
unsigned int counterC = 0;

void main(void)
{
    volatile int i,j;
    WDTCTL = WDTPW | WDTHOLD;       // Stop watchdog timer
    ConfigureClockModule();

    // Initialize hardware.
    InitializePushButtonPortPin();
    ConfigureTimerA();
    InitializeSPI();
    ConfigureADC();
    InitializeLEDDisplay();
    _enable_interrupts();

    //Run through the the calibration process. Xmax, Xmin, Ymax, Ymin, Zmax, Zmin.
    CalibrateACC();



    SetLEDDisplay(0x00);


    while (1) {
        counterA++;
        counterB++;
        counterC++;
        CalculateAngleHypPhi();

    }
}

void ConfigureClockModule()
{
    // Configure Digitally Controlled Oscillator (DCO) using factory calibrations
    DCOCTL  = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;
}

void CalculateAve(void){
    X_Ave = X_Axis_Sum >> 3;
    Y_Ave = Y_Axis_Sum >> 3;
    Z_Ave = Z_Axis_Sum >> 3;
}

void CalibrateACC(void){
    InitializePushButtonPortPin();
    SwitchDefine PushButton;
    // Initialize the pushbutton switch.
    InitializeSwitch(&PushButton,(char *) &PUSHBUTTON_PORT_IN,(unsigned char) PUSHBUTTON_BIT,
            HIGH_THRESHOLD,LOW_THRESHOLD);
    SetLEDDisplay(0x80);

    while(calibrationCount == 0){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Xmax = X_Ave;
        }
    }
    SetLEDDisplay(0x08);

    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 1){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Xmin = X_Ave;
        }
    }
    SetLEDDisplay(0x02);

    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 2){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Ymax = Y_Ave;
        }
    }
    SetLEDDisplay(0x20);

    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 3){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Ymin = Y_Ave;
        }
    }
    SetLEDDisplay(0xAA);

    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 4){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Zmax = Z_Ave;
        }
    }
    SetLEDDisplay(0x55);

    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 5){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Zmin = Z_Ave;
        }
    }
    //Set the Origin Point
    CalculateOrigins();
}

void CalculateOrigins(void){
    Xorigin = (Xmax + Xmin)/2;
    Yorigin = (Ymax + Ymin)/2;
    Zorigin = (Zmax + Zmin)/2;
}

void CenterXYZ(void){
    CalculateAve();
    X_Ave -= Xorigin;
    Y_Ave -= Yorigin;
    Z_Ave -= Zorigin;
}

void CalculateAngleHypPhi(void){
    Read_Adc();
    CenterXYZ();
    calculations calcs;
    if (X_Ave >= 0) { // first quadrant and fourth quadrant
        calcs.x = MUL*X_Ave; calcs.y = MUL*Y_Ave; calcs.angle = 0;
        Cordic((calculations *)&calcs,ATAN_HYP);
        angle = ((float) calcs.angle)/MUL;
    }
    else if (X_Ave < 0 && Y_Ave >= 0) { // second quadrant
        calcs.x = -1.0*MUL*X_Ave; calcs.y = MUL*Y_Ave; calcs.angle = 0;
        Cordic((calculations *)&calcs,ATAN_HYP);
        angle = 180.0 - ((float) calcs.angle)/MUL;
    }
    else if (X_Ave < 0 && Y_Ave < 0) { // third quadrant
        calcs.x = -1.0*MUL*X_Ave; calcs.y = -1.0*MUL*Y_Ave; calcs.angle = 0;
        Cordic((calculations *)&calcs,ATAN_HYP);
        angle = ((float) calcs.angle)/MUL - 180.0;
    }

    // X is adjusted by the CORDIC_GAIN, which equals 0x9B (155 decimal), for
    // MUL = 256 and CORDIC_GAIN = 0.607252935103139.  However, if X = Y,
    // then the CORDIC_GAIN = sqrt(2)/2.
    if (X_Ave == Y_Ave) {
        hypotenuse = ((float) calcs.x)*CORDIC_GAIN_X_EQUALS_Y/MUL;
    }
    else {
        hypotenuse = ((float) calcs.x)*CORDIC_GAIN/MUL;
    }

    //Calculate Phi
    if (hypotenuse >= 0) { // first quadrant and fourth quadrant
        calcs.x = MUL*hypotenuse; calcs.y = MUL*Z_Ave; calcs.angle = 0;
        Cordic((calculations *)&calcs,ATAN_HYP);
        phi = ((float) calcs.angle)/MUL;
    }
    else if (hypotenuse < 0 && Z_Ave >= 0) { // second quadrant
        calcs.x = -1.0*MUL*hypotenuse; calcs.y = MUL*Z_Ave; calcs.angle = 0;
        Cordic((calculations *)&calcs,ATAN_HYP);
        phi = 180.0 - ((float) calcs.angle)/MUL;
    }
    else if (hypotenuse < 0 && Z_Ave < 0) { // third quadrant
        calcs.x = -1.0*MUL*hypotenuse; calcs.y = -1.0*MUL*Z_Ave; calcs.angle = 0;
        Cordic((calculations *)&calcs,ATAN_HYP);
        phi = ((float) calcs.angle)/MUL - 180.0;
    }
    dispLED();
}

void ManageSoftwareTimers(void)
{
    if (g1msTimeout) {
        g1msTimeout--;
        g1msTimer++;

    }
}

void dispLED(void){
    if(angle <= 22.5 && angle >= -22.5){

        if ((phi >= 60 && phi <= 90) || (phi >=-90 && phi <= -60)){
            if(counterC >=40){
                SetLEDDisplay(0x80);
                counterC=0;
            }
            if (counterA >= 80){
               SetLEDDisplay(0x41);
               counterA = 0;
             }
            if (counterB >= 120){
               SetLEDDisplay(0x22);
               counterB = 0;
            }
        }
        if ((phi >= 30 && phi <= 60) || (phi >=-60 && phi <= -30)){
            if(counterC >=15){
                SetLEDDisplay(0x80);
                counterC=0;
            }
            if (counterA >= 30){
               SetLEDDisplay(0x41);
               counterA = 0;
             }
            if (counterB >= 45){
               SetLEDDisplay(0x22);
               counterB = 0;
            }
        }
        if ((phi >= 0 && phi <= 30) || (phi >=-30 && phi <= 0)){
            if(counterC >=1){
                SetLEDDisplay(0x80);
                counterC=0;
            }
            if (counterA >= 5){
               SetLEDDisplay(0x41);
               counterA = 0;
             }
            if (counterB >= 15){
               SetLEDDisplay(0x22);
               counterB = 0;
            }
        }

    }
    if(angle <= 67.5 && angle >= 22.5){
            if ((phi >= 60 && phi <= 90) || (phi >=-90 && phi <= -60)){
                if(counterC >= 40){
                    SetLEDDisplay(0x01);
                    counterC = 0;
                }
                if (counterA >= 80){
                  SetLEDDisplay(0x82);
                  counterA = 0;
                }
               if (counterB >= 120){
                  SetLEDDisplay(0x44);
                  counterB = 0;
               }
            }
            if ((phi >= 30 && phi <= 60) || (phi >=-60 && phi <= -30)){
                    if(counterC >= 15){
                        SetLEDDisplay(0x01);
                        counterC = 0;
                    }
                    if (counterA >= 30){
                      SetLEDDisplay(0x82);
                      counterA = 0;
                    }
                   if (counterB >= 45){
                      SetLEDDisplay(0x44);
                      counterB = 0;
                   }
                }
            if ((phi >= 0 && phi <= 30) || (phi >=-30 && phi <= 0)){
                    if(counterC >= 1){
                        SetLEDDisplay(0x01);
                        counterC = 0;
                    }
                    if (counterA >= 5){
                      SetLEDDisplay(0x82);
                      counterA = 0;
                    }
                   if (counterB >= 15){
                      SetLEDDisplay(0x44);
                      counterB = 0;
                   }
                }
        }
    if(angle <= 112.5 && angle >= 67.5){
            if ((phi >= 60 && phi <= 90) || (phi >=-90 && phi <= -60)){
                if(counterC >= 40){
                    SetLEDDisplay(0x02);
                    counterC = 0;
                }
                if (counterA >= 80){
                 SetLEDDisplay(0x05);
                 counterA = 0;
               }
              if (counterB >= 120){
                 SetLEDDisplay(0x88);
                 counterB = 0;
              }
            }
            if ((phi >= 30 && phi <= 60) || (phi >=-60 && phi <= -30)){
                    if(counterC >=15){
                        SetLEDDisplay(0x02);
                        counterC = 0;
                    }
                    if (counterA >= 30){
                     SetLEDDisplay(0x05);
                     counterA = 0;
                   }
                  if (counterB >= 45){
                     SetLEDDisplay(0x88);
                     counterB = 0;
                  }
                }
            if ((phi >= 0 && phi <= 30) || (phi >=-30 && phi <= 0)){
                    if(counterC >= 1){
                        SetLEDDisplay(0x02);
                        counterC = 0;
                    }
                    if (counterA >= 5){
                     SetLEDDisplay(0x05);
                     counterA = 0;
                   }
                  if (counterB >= 15){
                     SetLEDDisplay(0x88);
                     counterB = 0;
                  }
                }
        }
    if(angle <= 157.5 && angle >= 112.5){
            if ((phi >= 60 && phi <= 90) || (phi >=-90 && phi <= -60)){
                if(counterC >= 40){
                    SetLEDDisplay(0x04);
                    counterC = 0;
                }
                if (counterA >= 80){
                     SetLEDDisplay(0x0A);
                     counterA = 0;
                   }
                  if (counterB >= 120){
                     SetLEDDisplay(0x11);
                     counterB = 0;
                  }
            }
            if ((phi >= 30 && phi <= 60) || (phi >=-60 && phi <= -30)){
                    if(counterC >= 15){
                        SetLEDDisplay(0x04);
                        counterC = 0;
                    }
                    if (counterA >= 30){
                         SetLEDDisplay(0x0A);
                         counterA = 0;
                       }
                      if (counterB >= 45){
                         SetLEDDisplay(0x11);
                         counterB = 0;
                      }
                }
            if ((phi >= 0 && phi <= 30) || (phi >=-30 && phi <= 0)){
                if(counterC >= 1){
                    SetLEDDisplay(0x04);
                    counterC = 0;
                }
                if (counterA >= 5){
                     SetLEDDisplay(0x0A);
                     counterA = 0;
                   }
                  if (counterB >= 15){
                     SetLEDDisplay(0x11);
                     counterB = 0;
                  }
                }
        }
    if((angle <= 180 && angle >= 157.5) || (angle <= -157.5 && angle >= -180)){

          if ((phi >= 60 && phi <= 90) || (phi >=-90 && phi <= -60)){
              if(counterC >= 40){
                  SetLEDDisplay(0x08);
                  counterC = 0;
              }
              if (counterA >= 80){
                   SetLEDDisplay(0x14);
                   counterA = 0;
                 }
                if (counterB >= 120){
                   SetLEDDisplay(0x22);
                   counterB = 0;
                }
          }
          if ((phi >= 30 && phi <= 60) || (phi >=-60 && phi <= -30)){
                  if(counterC >= 15){
                      SetLEDDisplay(0x08);
                      counterC = 0;
                  }
                  if (counterA >= 30){
                       SetLEDDisplay(0x14);
                       counterA = 0;
                     }
                    if (counterB >= 45){
                       SetLEDDisplay(0x22);
                       counterB = 0;
                    }
              }
          if ((phi >= 0 && phi <= 30) || (phi >=-30 && phi <= 0)){
              if(counterC >= 1){
                  SetLEDDisplay(0x08);
                  counterC = 0;
              }
              if (counterA >= 5){
                   SetLEDDisplay(0x14);
                   counterA = 0;
                 }
                if (counterB >= 15){
                   SetLEDDisplay(0x22);
                   counterB = 0;
                }
              }
        }
    if(angle <= -112.5 && angle >= -157.5){
            if ((phi >= 60 && phi <= 90) || (phi >=-90 && phi <= -60)){
                  if(counterC >= 40){
                      SetLEDDisplay(0x10);
                      counterC = 0;
                  }
                  if (counterA >= 80){
                       SetLEDDisplay(0x28);
                       counterA = 0;
                     }
                    if (counterB >= 120){
                       SetLEDDisplay(0x44);
                       counterB = 0;
                    }
              }
              if ((phi >= 30 && phi <= 60) || (phi >=-60 && phi <= -30)){
                      if(counterC >= 15){
                          SetLEDDisplay(0x10);
                          counterC = 0;
                      }
                      if (counterA >= 30){
                           SetLEDDisplay(0x28);
                           counterA = 0;
                         }
                        if (counterB >= 45){
                           SetLEDDisplay(0x44);
                           counterB = 0;
                        }
                  }
              if ((phi >= 0 && phi <= 30) || (phi >=-30 && phi <= 0)){
                  if(counterC >= 1){
                      SetLEDDisplay(0x10);
                      counterC = 0;
                  }
                  if (counterA >= 5){
                       SetLEDDisplay(0x28);
                       counterA = 0;
                     }
                    if (counterB >= 15){
                       SetLEDDisplay(0x44);
                       counterB = 0;
                    }
                  }
        }
    if(angle <= -67.5 && angle >= -112.5){
              if ((phi >= 60 && phi <= 90) || (phi >=-90 && phi <= -60)){
                  if(counterC >= 40){
                      SetLEDDisplay(0x20);
                      counterC = 0;
                  }
                  if (counterA >= 80){
                       SetLEDDisplay(0x50);
                       counterA = 0;
                     }
                    if (counterB >= 120){
                       SetLEDDisplay(0x88);
                       counterB = 0;
                    }
              }
              if ((phi >= 30 && phi <= 60) || (phi >=-60 && phi <= -30)){
                      if(counterC >= 15){
                          SetLEDDisplay(0x20);
                          counterC = 0;
                      }
                      if (counterA >= 30){
                           SetLEDDisplay(0x50);
                           counterA = 0;
                         }
                        if (counterB >= 45){
                           SetLEDDisplay(0x88);
                           counterB = 0;
                        }
                  }
              if ((phi >= 0 && phi <= 30) || (phi >=-30 && phi <= 0)){
                  if(counterC >= 1){
                      SetLEDDisplay(0x20);
                      counterC = 0;
                  }
                  if (counterA >= 5){
                       SetLEDDisplay(0x50);
                       counterA = 0;
                     }
                    if (counterB >= 15){
                       SetLEDDisplay(0x88);
                       counterB = 0;
                    }
                  }
        }
    if(angle <= -22.5 && angle >= -67.5){
              if ((phi >= 60 && phi <= 90) || (phi >=-90 && phi <= -60)){
                  if(counterC >= 40){
                      SetLEDDisplay(0x40);
                      counterC = 0;
                  }
                  if (counterA >= 80){
                       SetLEDDisplay(0xA0);
                       counterA = 0;
                     }
                    if (counterB >= 120){
                       SetLEDDisplay(0x11);
                       counterB = 0;
                    }
              }
              if ((phi >= 30 && phi <= 60) || (phi >=-60 && phi <= -30)){
                      if(counterC >= 15){
                          SetLEDDisplay(0x40);
                          counterC = 0;
                      }
                      if (counterA >= 30){
                           SetLEDDisplay(0xA0);
                           counterA = 0;
                         }
                        if (counterB >= 45){
                           SetLEDDisplay(0x11);
                           counterB = 0;
                        }
                  }
              if ((phi >= 0 && phi <= 30) || (phi >=-30 && phi <= 0)){
                  if(counterC >= 1){
                      SetLEDDisplay(0x40);
                      counterC = 0;
                  }
                  if (counterA >= 5){
                       SetLEDDisplay(0xA0);
                       counterA = 0;
                     }
                    if (counterB >= 15){
                       SetLEDDisplay(0x11);
                       counterB = 0;
                    }
                  }
        }

}
