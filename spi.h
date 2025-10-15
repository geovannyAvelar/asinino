#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <stdint.h>

void networking_spi_init(void);
uint8_t networking_spi_transfer(uint8_t data);

#endif // SPI_H