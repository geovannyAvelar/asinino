#include "console.h"
#include "string.h"
#include "mem.h"
#include "mcu.h"

void console_init(void)
{
  uart_print("Asinino v0.0.1!\r\n");

  while (1)
  {
    uart_print("> ");
    char *command = (char *) get_page();
    uart_read_line(command, 128);

    if (strcmp(command, "clear") == 0)
    {
      uart_print(CLEAR_CONSOLE);
    }

    if (strcmp(command, "restart") == 0)
    {
      uart_print("Restarting...\r\n\n\n");
      mcu_restart();
    }

    if (strcmp(command, "dumppt") == 0)
    {
      dump_page_table();
    }

    free_page(command);

    uart_print("\r\n");
  }
}