#include "spi.h"
#include "uart.h"

void spi_init(void)
{
  uart_print("Initializing SPI...\r\n");

  DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
  DDRB &= ~(1 << PB4);

  SPCR = (1 << SPE) | (1 << MSTR);

  uart_print("SPI Initialized!\r\n");
}

uint8_t spi_transfer(uint8_t data)
{
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
    ;
  return SPDR;
}