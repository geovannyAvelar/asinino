#include "mem.h"
#include "uart.h"
#include "console.h"
#include "spi.h"
#include "networking/networking.h"

int main(void)
{
  uart_init(UART_BAUD_RATE);

  uart_print("Asinino v0.0.1!\r\n");
  uart_print("Booting up...\r\n");

  meminit();
  spi_init();
  networking_init();

  uart_print("System initialized.\r\n");

  console_init();

  return 0;
}
