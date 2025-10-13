#include "mcu.h"
#include "ds3231.h"

static uint32_t startup_time_ms = 0;

void mcu_restart(void)
{
  void (*restart_mcu)(void) = 0x0000;
  restart_mcu();
}

void set_startup_time(void)
{
  uint32_t ms = ds3231_read_time_ms();
  startup_time_ms = ms;
}

uint32_t mcu_get_uptime_ms(void)
{
  uint32_t current_time = ds3231_read_time_ms();
  return current_time - startup_time_ms;
}