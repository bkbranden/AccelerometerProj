#ifndef ADC_H
#define ADC_H
#include <msp430.h>

// Prototypes
void ConfigureADC(void);
void Read_Adc(void);

#define USCIA0_X_BIT                 BIT0
#define USCIA0_X_PORT                P1IN
#define USCIA0_X_DDR                 P1DIR
#define SET_XOUT_AS_AN_INPUT       USCIA0_X_DDR &= ~USCIA0_X_BIT

#define USCIA0_Y_BIT                 BIT1
#define USCIA0_Y_PORT                P1IN
#define USCIA0_Y_DDR                 P1DIR
#define SET_YOUT_AS_AN_INPUT        USCIA0_Y_DDR &= ~USCIA0_Y_BIT

#define USCIA0_Z_BIT                 BIT2
#define USCIA0_Z_PORT                P1IN
#define USCIA0_Z_DDR                 P1DIR
#define SET_ZOUT_AS_AN_INPUT         USCIA0_Z_DDR &= ~USCIA0_Z_BIT

#endif
