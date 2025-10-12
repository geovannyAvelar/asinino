#include "console.h"
#include "asi_string.h"
#include "mem.h"
#include "mcu.h"

void console_init(void)
{
  uart_print("Asinino v0.0.1!\r\n");

  while (1)
  {
    uart_print("> ");
    char *input = (char *)get_page();
    uart_read_line(input, 128);

    char *args[7];
    uint8_t count = parse_command_args(input, args);

    char *command = args[0];

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

    free_page(args);
    free_page(input);

    uart_print("\r\n");
  }
}

uint8_t parse_command_args(char *input, char *args[7])
{
  uint8_t count = 0;
  for (int i = 0; i < 7; i++)
  {
    strsplit(input, ' ', i, args[i]);

    if (args[i][0] == '\0')
    {
      break;
    }

    count++;
  }

  return count;
}
