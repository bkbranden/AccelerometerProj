#include "spi.h"

void InitializeSPI()
{
    SET_USCIA0_MOSI_AS_AN_OUTPUT;
    SET_SPI_SCK_AS_AN_OUTPUT;
    SET_0_MOSI;
    SET_0_GPIO;
    SET_LATCH_AS_AN_OUTPUT;
    SET_0_LATCH;
    SET_BLANK_AS_AN_OUTPUT;
    SET_0_BLANK;
}

void SPISendByte(unsigned char SendValue)
{
    volatile unsigned char SendValueCopy = SendValue;
    int i;
    for(i = 0; i < 8; i++){
        if(SendValueCopy & BIT7){
            USCIA0_MOSI_PORT |= USCIA0_MOSI_BIT;
        }
        else{
            USCIA0_MOSI_PORT &= ~USCIA0_MOSI_BIT;
        }
        SendValueCopy<<=1;
        SPI_SCK_PORT |= SPI_SCK_BIT;
        SPI_SCK_PORT &= ~SPI_SCK_BIT;
    }
    USCIA0_MOSI_PORT &= ~USCIA0_MOSI_BIT;
}
