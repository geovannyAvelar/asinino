#ifndef MCU_H
#define MCU_H

#include <stdint.h>

void mcu_restart(void);
void set_startup_time(void);
uint32_t mcu_get_uptime_ms(void);

#endif // MCU_H