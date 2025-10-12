#include "mcu.h"

void mcu_restart(void)
{
  void (*restart_mcu)(void) = 0x0000;
  restart_mcu();
}