#ifndef W5100_H
#define W5100_H

#include <avr/io.h>
#include <stdint.h>

#define W5100_SS_PORT PORTB
#define W5100_SS_DDR  DDRB
#define W5100_SS_PIN  PB2

#define W5100_SELECT()   (W5100_SS_PORT &= ~(1 << W5100_SS_PIN))
#define W5100_DESELECT() (W5100_SS_PORT |=  (1 << W5100_SS_PIN))

uint8_t networking_spi_transfer(uint8_t data);
void w5100_init(const uint8_t mac[6], const uint8_t ip[4], const uint8_t gateway[4], const uint8_t subnet[4]);
void w5100_write(uint16_t addr, uint8_t data);
uint8_t w5100_read(uint16_t addr);
void networking_spi_init(void);

#endif // W5100_H