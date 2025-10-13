#include "mem.h"
#include "uart.h"
#include "console.h"
#include "spi.h"
#include "networking/networking.h"

int main(void)
{
  uart_init(UART_BAUD_RATE);
  meminit();
  spi_init();
  networking_init();
  console_init();

  return 0;
}
