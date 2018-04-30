#include "spi.h"

void InitializeSPIPortPins()
{
    // Configure port pins
    SCLK_AS_AN_OUTPUT;

    // Configure port pin to receive output from USCI A0 clock.
    P1SEL2 |= SPI_SCK_BIT;
    P1SEL |= SPI_SCK_BIT;

    WRITE_LOGIC_0_TO_SLAVE;
    SET_USCIB0_MOSI_AS_AN_OUTPUT;

    // Configure port pin to receive output from USCI A0 MOSI.
    P1SEL2 |= USCIB0_MOSI_BIT;
    P1SEL |= USCIB0_MOSI_BIT;

    SET_USCIB0_MOSI_AS_AN_OUTPUT;
}

void InitializeSPI()
{
    // Software reset enabled. USCI logic held in reset state.
    UCB0CTL1 = UCSWRST;

    // Initialize all USCI registers with UCSWRST = 1 (including UCxCTL1)

    // Select USCI SPI functionality.
    UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC;
    UCB0CTL1 |= UCSSEL_2; // SMCLK as source
    UCB0BR1 = 0x03;
    UCB0BR0 |= 0xE8;   // divide clock by 1000 to get 16k Hz
    UCB0TXBUF = 0;                  // initialize transmit buffer to 0

    InitializeSPIPortPins();


    // Software reset disabled. USCI logic released for operation.
    UCB0CTL1 &= ~UCSWRST;
}

void SPISendByte(unsigned char byte_value)
{
    UCB0TXBUF = byte_value;
    while (UCB0STAT & UCBUSY);
}
