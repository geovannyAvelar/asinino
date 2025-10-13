#include "log.h"
#include "uart.h"
#include "ds3231.h"

void info(const char *message) {
  _print_message("\033[32mINFO\033[0m", message);
}

void error(const char *message) {
  _print_message("\033[31mERROR\033[0m", message);
}

void warn(const char *message) {
  _print_message("\033[33mWARN\033[0m", message);
}

void debug(const char *message) {
  _print_message("\033[34mDEBUG\033[0m", message);
}

void _print_message(const char *level, const char *message) {
  _print_date_time();
  uart_print(" - ");
  uart_print(level);
  uart_print(": ");
  uart_print(message);
  uart_print("\r\n");
}

void _print_date_time(void) {
  ds3231_time_t current_time;
  ds3231_read_time(&current_time);
  ds3231_print_time(&current_time);
}