#include "spi.h"
#include "log.h"

#include <avr/io.h>

void spi_init(void)
{
  info("Initializing SPI...");

  DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
  DDRB &= ~(1 << PB4);

  SPCR = (1 << SPE) | (1 << MSTR);

  info("SPI Initialized!");
}

uint8_t spi_transfer(uint8_t data)
{
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
    ;
  return SPDR;
}