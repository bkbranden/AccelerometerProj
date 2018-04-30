#ifndef LED_DISPLAY_H_
#define LED_DISPLAY_H_

#include <msp430.h>
#include "spi.h"

#define LED_DISPLAY_RESET_VALUE 0
#define LED_DISPLAY_BASE 10
#define LED_DISPLAY_MAX 99

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * LED Display Driver Latch (XLAT)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P2.0
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define LED_DISPLAY_DRIVER_LATCH_BIT                BIT0
#define LED_DISPLAY_DRIVER_LATCH_PORT               P2OUT
#define LED_DISPLAY_DRIVER_LATCH_DDR                P2DIR
#define SET_LED_DISPLAY_DRIVER_LATCH_AS_AN_OUTPUT   LED_DISPLAY_DRIVER_LATCH_DDR |= LED_DISPLAY_DRIVER_LATCH_BIT
#define SET_LED_DISPLAY_DRIVER_LATCH                LED_DISPLAY_DRIVER_LATCH_PORT |= LED_DISPLAY_DRIVER_LATCH_BIT
#define RESET_LED_DISPLAY_DRIVER_LATCH              LED_DISPLAY_DRIVER_LATCH_PORT &= ~LED_DISPLAY_DRIVER_LATCH_BIT

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * LED Display Driver Blank
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.4
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define LED_DISPLAY_DRIVER_BLANK_BIT                BIT4
#define LED_DISPLAY_DRIVER_BLANK_PORT               P1OUT
#define LED_DISPLAY_DRIVER_BLANK_DDR                P1DIR
#define SET_LED_DISPLAY_DRIVER_BLANK_AS_AN_OUTPUT   LED_DISPLAY_DRIVER_BLANK_DDR |= LED_DISPLAY_DRIVER_BLANK_BIT
#define ENABLE_LED_DISPLAY_DRIVER_BLANK             LED_DISPLAY_DRIVER_BLANK_PORT &= ~LED_DISPLAY_DRIVER_BLANK_BIT
#define DISABLE_LED_DISPLAY_DRIVER_BLANK            LED_DISPLAY_DRIVER_BLANK_PORT |= LED_DISPLAY_DRIVER_BLANK_BIT



// Function Prototypes
void InitializeLEDDisplay(void);
void SetLEDDisplay(unsigned char DisplayValue);

#endif /* LED_DISPLAY_H_ */
