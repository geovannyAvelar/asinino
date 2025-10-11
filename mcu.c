#include "mcu.h"

void (*restart_mcu)(void) = 0x0000;

void mcu_restart(void)
{
  restart_mcu();
}