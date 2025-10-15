#include "console.h"
#include "uart.h"
#include "asi_string.h"
#include "mcu.h"
#include "ds3231.h"
#include "log.h"
#include <stdlib.h>
#include <stdint.h>

void console_init(void)
{
  info("Console initialized");

  while (1)
  {
    uart_print("> ");
    char input[128];
    uart_read_line(input, 128);

    char *args[16];
    uint8_t count = parse_command_args(input, args);

    char *command = args[0];

    uint8_t is_clear_command = (input[0] == 12 && input[1] == '\0');
    if (is_clear_command || strcmp(command, "clear") == 0 || strcmp(command, "cls") == 0)
    {
      uart_print(CLEAR_CONSOLE);
    }

    if (strcmp(command, "restart") == 0)
    {
      warn("Restarting...");
      mcu_restart();
    }

    if (strcmp(command, "echo") == 0)
    {
      if (count < 2)
      {
        uart_print("Usage: echo <message>");
      }
      else
      {
        for (int i = 1; i < count; i++)
        {
          uart_print(args[i]);
          if (i < count - 1)
          {
            uart_print(" ");
          }
        }
      }
    }

    if (strcmp(command, "uptime") == 0)
    {
      uint32_t uptime = mcu_get_uptime_ms();

      uint32_t seconds = uptime / 1000;
      uint32_t days = seconds / 86400;
      uint32_t hours = (seconds % 86400) / 3600;
      uint32_t minutes = (seconds % 3600) / 60;
      uint32_t secs = seconds % 60;

      uart_print("Uptime: ");
      uart_print_int(days);
      uart_print("d ");
      uart_print_int(hours);
      uart_print("h ");
      uart_print_int(minutes);
      uart_print("m ");
      uart_print_int(secs);
      uart_print("s\n");
    }

    if (strcmp(command, "date") == 0)
    {
      print_date();
    }

    if (strcmp(command, "setdate") == 0)
    {
      if (count != 7)
      {
        uart_print("Usage: setdate <sec> <min> <hour> <day_of_month> <month> <year>");
      }
      else
      {
        ds3231_time_t new_time = {0};
        new_time.sec = (uint8_t)atoi(args[1]);
        new_time.min = (uint8_t)atoi(args[2]);
        new_time.hour = (uint8_t)atoi(args[3]);
        new_time.day_of_week = 0;
        new_time.day_of_month = (uint8_t)atoi(args[4]);
        new_time.month = (uint8_t)atoi(args[5]);
        new_time.year = (uint8_t)atoi(args[6]) - 2000; // Assuming year is given as YYYY

        ds3231_set_time(&new_time);
        uart_print("Date and time updated to: ");
        ds3231_print_time(&new_time);
        uart_print("\r\n");
      }
    }

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

void print_date(void)
{
  ds3231_time_t current_time;
  ds3231_read_time(&current_time);
  ds3231_print_time(&current_time);
}