#include "uart.h"
#include "types.h"

#define CLEAR_CONSOLE "\033[2J\033[H"

void console_init(void);
void parse_input_command(char *input, char *command);
uint8_t parse_command_args(char *input, char *args[7]);