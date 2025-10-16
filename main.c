#include "mcu.h"
#include "uart.h"
#include "log.h"
#include "ds3231.h"
#include "console.h"
#include "networking/networking.h"
#include "fs/diskio.h"

void banner(void);

int main(void)
{
  uart_init(UART_BAUD_RATE);
  ds3231_init();
  set_startup_time();

  banner();

  if (disk_initialize(0) != RES_OK)
  {
    error("Disk initialization failed");
  } else {
    info("Disk initialized successfully");
  }

  networking_init();

  console_init();

  return 0;
}

void banner(void)
{
  uart_print(
      "                          /\\          /\\\r\n"
      "                         ( \\\\        // )\r\n"
      "                          \\ \\\\      // /\r\n"
      "                           \\_\\\\||||//_/\r\n"
      "                            \\/ _  _ \\\r\n"
      "                           \\/|(O)(O)|\r\n"
      "                          \\/ |      |\r\n"
      "      ___________________\\/  \\      /\r\n"
      "     //                //     |____|\r\n"
      "    //                ||     /      \\\r\n"
      "   //|                \\|     \\ 0  0 / Asinino v0.0.1!\r\n"
      "  // \\       )         V    / \\____/\r\n"
      " //   \\     /        (     /\r\n"
      "\"\"     \\   /_________|  |_/\r\n"
      "       /  /\\   /     |  ||\r\n"
      "      /  / /  /      \\  ||\r\n"
      "      | |  | |        | ||\r\n"
      "      | |  | |        | ||\r\n"
      "      |_|  |_|        |_||\r\n"
      "       \\_\\  \\_\\        \\_\\\\ Hard'96\r\n");
  uart_print("ASININO - An educational OS for learning purposes\r\n");
}
