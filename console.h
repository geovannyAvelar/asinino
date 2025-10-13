#ifndef CONSOLE_H
#define CONSOLE_H

#include "uart.h"
#include <stdint.h>

#define CLEAR_CONSOLE "\033[2J\033[H"

void console_init(void);
void parse_input_command(char *input, char *command);
uint8_t parse_command_args(char *input, char *args[7]);
void print_date(void);

#endif // CONSOLE_H