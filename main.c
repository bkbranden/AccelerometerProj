#include <msp430.h> 
#include <math.h>
#include "cordic.h"
#include "pushbutton.h"
#include "timerA.h"
#include "spi.h"
#include "ADC.h"
#include "debounce.h"

#define CORDIC_GAIN 0.607252935103139
#define CORDIC_GAIN_X_EQUALS_Y (float) (sqrt(2)/2)
#define PI 3.1415926536897932384626

// Function Prototypes
void ConfigureClockModule();
void CalculateAve(void);
void CalibrateACC(void);
void CalculateOrigins(void);
void CenterXYZ(void);
void CalculateAngleHyp(void);

// Global variables
unsigned int g1msTimer = 0;
unsigned int calibrationCount = 0;
unsigned int X_Axis_Sum, Y_Axis_Sum, Z_Axis_Sum;
signed int X_Ave, Y_Ave, Z_Ave;
unsigned int Xmax, Xmin, Ymax, Ymin, Zmax, Zmin;
unsigned int Xorigin, Yorigin, Zorigin;
volatile float angle, hypotenuse, phi;

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

    _enable_interrupts();

    //Run through the the calibration process. Xmax, Xmin, Ymax, Ymin, Zmax, Zmin.
    CalibrateACC();

    //Set the Origin Point
    CalculateOrigins();

    SPISendByte(0x00);
    TOGGLE_LATCH;

    while (1) {
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
    SPISendByte(0x80);
    TOGGLE_LATCH;
    while(calibrationCount == 0){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Xmax = X_Ave;
        }
    }
    SPISendByte(0x08);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 1){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Xmin = X_Ave;
        }
    }
    SPISendByte(0x02);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 2){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Ymax = Y_Ave;
        }
    }
    SPISendByte(0x20);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 3){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Ymin = Y_Ave;
        }
    }
    SPISendByte(0xAA);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 4){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Zmax = Z_Ave;
        }
    }
    SPISendByte(0x55);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 5){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            CalculateAve();
            Zmin = Z_Ave;
        }
    }
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
    _nop();
}
