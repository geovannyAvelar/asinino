#include "w5100.h"

#include <util/delay.h>
#include "../spi.h"

void w5100_write(uint16_t addr, uint8_t data)
{
  W5100_SELECT();
  spi_transfer(0xF0); // Write command
  spi_transfer(addr >> 8);
  spi_transfer(addr & 0xFF);
  spi_transfer(data);
  W5100_DESELECT();
}

uint8_t w5100_read(uint16_t addr)
{
  uint8_t data;
  W5100_SELECT();
  spi_transfer(0x0F); // Read command
  spi_transfer(addr >> 8);
  spi_transfer(addr & 0xFF);
  data = spi_transfer(0x00);
  W5100_DESELECT();
  return data;
}

void w5100_init(const uint8_t mac[6], const uint8_t ip[4], const uint8_t gateway[4],
                const uint8_t subnet[4])
{
  w5100_write(0x0000, 0x80);
  _delay_ms(1);

  for (uint8_t i = 0; i < 4; i++)
    w5100_write(0x0001 + i, gateway[i]);

  for (uint8_t i = 0; i < 4; i++)
    w5100_write(0x0005 + i, subnet[i]);

  for (uint8_t i = 0; i < 6; i++)
    w5100_write(0x0009 + i, mac[i]);

  for (uint8_t i = 0; i < 4; i++)
    w5100_write(0x000F + i, ip[i]);
}
