#include "spi.h"

void spi_init(void)
{
  DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
  DDRB &= ~(1 << PB4);

  SPCR = (1 << SPE) | (1 << MSTR);
}

uint8_t spi_transfer(uint8_t data)
{
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
    ;
  return SPDR;
}