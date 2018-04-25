#include "ADC.h"

unsigned int adcX[8], adcY[8], adcZ[8], adc[3];
extern unsigned int X_Axis_Sum, Y_Axis_Sum, Z_Axis_Sum;
unsigned int n;

void ConfigureADC(void)
{
    ADC10CTL0 &= ~ENC;

    ADC10CTL1 = INCH_2 + CONSEQ_1 + ADC10SSEL_0;
    ADC10CTL0 = ADC10SHT_2 + MSC + ADC10ON + SREF_0;
    ADC10DTC1 = 0x03;
    ADC10AE0 |= 0x07;
    ADC10CTL0 |= (ENC | ADC10SC);
}

void Read_Adc(void){
    while (ADC10CTL1 & BUSY);
    ADC10SA = (short) &adc[0];
    ADC10CTL0 |= ADC10SC;

    n++;
    if(n > 7){
        n = 0;
    }
    Z_Axis_Sum -= adcZ[n];
    Z_Axis_Sum += adc[0];
    adcZ[n] = adc[0];
    Y_Axis_Sum -= adcY[n];
    Y_Axis_Sum += adc[1];
    adcY[n] = adc[1];
    X_Axis_Sum -= adcX[n];
    X_Axis_Sum += adc[2];
    adcX[n] = adc[2];
}
