#include "console.h"
#include "asi_string.h"
#include "mem.h"
#include "mcu.h"
#include "uart.h"
#include "ds3231.h"
#include "log.h"

void console_init(void)
{
  info("Console initialized");

  while (1)
  {
    uart_print("> ");
    char *input = (char *)get_page();
    uart_read_line(input, 128);

    char *args[16];
    uint8_t count = parse_command_args(input, args);

    char *command = args[0];

    if (strcmp(command, "clear") == 0)
    {
      uart_print(CLEAR_CONSOLE);
    }

    if (strcmp(command, "restart") == 0)
    {
      warn("Restarting...");
      mcu_restart();
    }

    if (strcmp(command, "dumppt") == 0)
    {
      dump_page_table();
    }

    if (strcmp(command, "date") == 0)
    {
      ds3231_time_t* current_time = (ds3231_time_t*)get_page();
      ds3231_read_time(current_time);
      ds3231_print_time(current_time);
      free_page(current_time);
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
