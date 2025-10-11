#include "mem.h"
#include "uart.h"
#include "console.h"

int main(void)
{
  uart_init(UART_BAUD_RATE);
  meminit();

  console_init();

  return 0;
}
