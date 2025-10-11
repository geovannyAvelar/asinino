#include "console.h"

void console_init(void)
{
  uart_init(UART_BAUD_RATE);
  uart_print("Asinino v0.0.1!\r\n");

  while (1) {
    uart_print("> ");
    char *command = uart_read_line();
    uart_print("\r\n");
  }
}