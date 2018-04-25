#include <msp430.h> 

#include "pushbutton.h"
#include "timerA.h"
#include "spi.h"
#include "ADC.h"
#include "debounce.h"

// Function Prototypes
void ConfigureClockModule();
void calculateAve(void);
void CalibrateACC(void);

// Global variables
unsigned int g1msTimer = 0;
unsigned int calibrationCount = 0;
unsigned int X_Axis_Sum, Y_Axis_Sum, Z_Axis_Sum;
unsigned int X_Ave, Y_Ave, Z_Ave;
unsigned int Xmax, Xmin, Ymax, Ymin, Zmax, Zmin;

void main(void)
{
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

    SPISendByte(0x00);
    TOGGLE_LATCH;

    while (1) {
        // First, update software timers.
    }
}

void ConfigureClockModule()
{
    // Configure Digitally Controlled Oscillator (DCO) using factory calibrations
    DCOCTL  = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;
}

void calculateAve(void){
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
            calculateAve();
            Xmax = X_Ave;
        }
    }
    SPISendByte(0x08);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 1){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            calculateAve();
            Xmin = X_Ave;
        }
    }
    SPISendByte(0x02);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 2){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            calculateAve();
            Ymax = Y_Ave;
        }
    }
    SPISendByte(0x20);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 3){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            calculateAve();
            Ymin = Y_Ave;
        }
    }
    SPISendByte(0xAA);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 4){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            calculateAve();
            Zmax = Z_Ave;
        }
    }
    SPISendByte(0x55);
    TOGGLE_LATCH;
    while(Debouncer(&PushButton) == High);
    while(calibrationCount == 5){
        if(Debouncer(&PushButton) == High){
            calibrationCount++;
            calculateAve();
            Zmin = Z_Ave;
        }
    }
}
