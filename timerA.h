#ifndef TIMER_A_H
#define TIMER_A_H

#include <msp430.h>

#define TA0CCR0_VALUE 1999  // Timer A period is TACCR0+1

// Prototypes
void ConfigureTimerA(void);

#endif
