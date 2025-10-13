#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>

#define F_CPU 16000000UL   // Assuming 16MHz clock
#define SCL_CLOCK 100000UL // 100kHz I2C clock

#define DS3231_ADDR_W 0xD0 // (0x68 << 1) | 0
#define DS3231_ADDR_R 0xD1 // (0x68 << 1) | 1


typedef struct __attribute__((packed, aligned(1))) ds3231_time_t
{
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t day_of_week;
  uint8_t day_of_month;
  uint8_t month;
  uint8_t year;
} ds3231_time_t;

void ds3231_init(void);
void ds3231_wait(void);
void ds3231_start(void);
void ds3231_stop(void);
uint8_t ds3231_write(uint8_t data);
uint8_t ds3231_read_ack(void);
uint8_t ds3231_read_nack(void);
void ds3231_read_time(ds3231_time_t *time);
uint32_t ds3231_read_time_ms(void);
void ds3231_set_time(const ds3231_time_t *time);
void ds3231_print_time(const ds3231_time_t *time);
uint8_t dec_to_bcd(uint8_t dec);
uint8_t bcd_to_dec(uint8_t bcd);

#endif // DS3231_H