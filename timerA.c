#include "timerA.h"
#include "LED_display.h"
#include "cordic.h"

//extern unsigned int g1msTimeout;
extern unsigned int dir = 0;
unsigned int duty = 32000;
unsigned char ones = 0x04;
//unsigned int counterA = 0;
//unsigned int counterB = 0;
//unsigned int counterC = 0;
extern unsigned int g1msTimeout;
extern unsigned int g1msTimer;
void ConfigureTimerA(void)
{
    // Divide the clock by 8
    //BCSCTL2 |= 0x04;

    // Timer0_A3 Control Register
    TA0CTL |= MC_0;     // Stop the timer
    TA0CTL |= TACLR;    // Clear the timer

    TA0CTL |= TASSEL_2 | ID_3 | MC_1;
    /* TASSEL1 = 0x0200 Timer A clock source select 0
     * TASSEL0 = 0x0100 Timer A clock source select 1
     *  00 = TACLK
     *  01 = ACLK
     *  10 = SMCLK
     *  11 = INCLK
     * ID1 = 0x0080 Timer A clock input divider 1
     * ID0 = 0x0040 Timer A clock input divider 0
     *  00 => Input divider = 1
     *  01 => Input divider = 2
     *  10 => Input divider = 4
     *  11 => Input divider = 8
     * MC1 = 0x0020 Timer A mode control 1
     * MC0 = 0x0010 Timer A mode control 0
     *  00 = Stop
     *  01 = Up
     *  10 = Continuous
     *  11 = Up/Down
     * TACLR = 0x0004 Timer A counter clear
     * TAIE = 0x0002 Timer A counter interrupt enable
     * TAIFG = 0x0001 Timer A counter interrupt flag
     */

    // Assign a value to TA0CCR0
    TA0CCR0 = TA0CCR0_VALUE;
    TA0CCR1 = 0;


    // Enable TACCR0 Compare/Capture Interrupt Flag (CCIFG0)
    TACTL |= TAIE;
    TA0CCTL0 |= CCIE;
    TA0CCTL1 |= CCIE;
}

#pragma vector = TIMER0_A0_VECTOR
// Interrupt service routine for CCIFG0
    __interrupt void Timer0_A0_routine(void){
        Read_Adc();
        g1msTimer++;
     //   if (x == 0){
        if (dir == 0){
            TA0CCR1 += 1;
            if (duty == TA0CCR1){
                dir = 1;
            }
        }
        else{
            TA0CCR1 -= 1;
            if (TA0CCR1 == 0){
                dir = 0;

            }
        }
//        }
//        else {
//
//        }

}

#pragma vector = TIMER0_A1_VECTOR
// Interrupt service routine for CCIFG1 and TAIFG
    __interrupt void Timer0_A1_routine(void)
{
    switch (TAIV){
    case TA0IV_NONE:
        break;
    case TA0IV_TACCR1: // CCIFG1 interrupt
        //SetLEDDisplay(0);
        break;
    case TA0IV_TAIFG: // TAIFG interrupt
//        SetLEDDisplay(0x07);
//        SetLEDDisplay(0x70);
//        counterA++;
//        counterB++;
//        counterC++;
//        if (counterA == 5){
//            if (ones == 0x01){
//                SetLEDDisplay(0x80);
//                SetLEDDisplay(ones <<1);
//
//            }
//            else if(ones == 0x80){
//                SetLEDDisplay(0x01);
//                SetLEDDisplay(ones >> 1);
//            }
//            else{
//                SetLEDDisplay(ones >>1);
//                SetLEDDisplay(ones <<1);
//            }
//            counterA = 0;
//        }
//        if (counterB == 20){
//            if (ones == 0x01){
//               SetLEDDisplay(0x40);
//               SetLEDDisplay(ones <<2);
//
//           }
//           else if(ones == 0x80){
//               SetLEDDisplay(0x02);
//               SetLEDDisplay(ones >> 2);
//           }
//           else if(ones == 0x40){
//              SetLEDDisplay(0x01);
//              SetLEDDisplay(ones >> 2);
//          }
//           else if(ones == 0x02){
//             SetLEDDisplay(0x80);
//             SetLEDDisplay(ones << 2);
//         }
//           else{
//               SetLEDDisplay(ones >>2);
//               SetLEDDisplay(ones <<2);
//           }
//            counterB = 0;
//        }
//        SetLEDDisplay(ones);
//
//                //SetLEDDisplay(ones);
//
//        if (counterC == 500){
//            if (ones == (0x80)){
//                ones = 0x01;
//            }
//            else{
//                ones = ones << 1;
//            }
//            counterC = 0;
//        }



        break;
    default: for (;;); // Should not be possible
    }
}

