#include "console.h"
#include "string.h"
#include "mcu.h"

void console_init(void)
{
  uart_print("Asinino v0.0.1!\r\n");

  while (1) {
    uart_print("> ");
    char *command = uart_read_line();

    if (strcmp(command, "restart") == 0) {
      uart_print("Restarting...\r\n");
      mcu_restart();
    }

    uart_print("\r\n");
  }
}