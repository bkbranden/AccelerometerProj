#include "LED_display.h"
#include "spi.h"

const unsigned char LEDDisplayCharacters[16] = {
        0x3f, // 0
        0x06, // 1
        0x5B, // 2
        0x4F, // 3
        0x66, // 4
        0x6D, // 5
        0x7D, // 6
        0x07, // 7
        0x7F, // 8
        0x67, // 9
        0x77, // A
        0x7C, // B
        0x39, // C
        0x5E, // D
        0x79, // E
        0x71  // F
};

void InitializeLEDDisplay(void)
{
    SET_LED_DISPLAY_DRIVER_LATCH_AS_AN_OUTPUT;
    SET_LED_DISPLAY_DRIVER_BLANK_AS_AN_OUTPUT;
    DISABLE_LED_DISPLAY_DRIVER_BLANK;
    RESET_LED_DISPLAY_DRIVER_LATCH;
}

void SetLEDDisplay(unsigned char DisplayValue)
{
//    unsigned char left_digit;
//    unsigned char right_digit;
//
//    left_digit = 0;
//    right_digit = DisplayValue;
//    while (right_digit > (LED_DISPLAY_BASE-1)) {
//        right_digit -= LED_DISPLAY_BASE;
//        left_digit++;
//    }
//
//    SPISendByte(LEDDisplayCharacters[right_digit]); // Send right display character
//    SPISendByte(LEDDisplayCharacters[left_digit]); // Send left display character

    SPISendByte(DisplayValue);
    SET_LED_DISPLAY_DRIVER_LATCH;
    RESET_LED_DISPLAY_DRIVER_LATCH;
    ENABLE_LED_DISPLAY_DRIVER_BLANK;

}

