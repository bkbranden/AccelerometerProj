#ifndef USCI_SPI_H
#define USCI_SPI_H

#include <msp430.h>

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USCI A0 Master-Out-Slave-In (MOSI)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.7
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define USCIB0_MOSI_BIT                 BIT7
#define USCIB0_MOSI_PORT                P1OUT
#define USCIB0_MOSI_DDR                 P1DIR
#define WRITE_LOGIC_1_TO_SLAVE          USCIB0_MOSI_PORT |= USCIB0_MOSI_BIT
#define WRITE_LOGIC_0_TO_SLAVE          USCIB0_MOSI_PORT &= ~USCIB0_MOSI_BIT
#define SET_USCIB0_MOSI_AS_AN_OUTPUT    USCIB0_MOSI_DDR |= USCIB0_MOSI_BIT


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * SCLK
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.5
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define SPI_SCK_BIT                   BIT5
#define SCLK_PORT                   P1OUT // SPI Slave Clock output
#define SCLK_DDR                    P1DIR // SPI Slave Clock direction
#define SCLK_AS_AN_OUTPUT           SCLK_DDR |= SPI_SCK_BIT

void InitializeSPIPortPins();
void InitializeSPI();
void SPISendByte(unsigned char byte_value);

#endif
