#include "ds3231.h"
#include "uart.h"
#include <avr/io.h>
#include <util/delay.h>

void ds3231_init(void)
{
  TWSR = 0x00;
  TWBR = (F_CPU / SCL_CLOCK - 16) / 2;
  TWCR = (1 << TWEN);
}

void ds3231_wait(void)
{
  while (!(TWCR & (1 << TWINT)))
  {
    _delay_ms(100);
  }
}

void ds3231_start(void)
{
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  ds3231_wait();
}

void ds3231_stop(void)
{
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

uint8_t ds3231_write(uint8_t data)
{
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  ds3231_wait();
  return (TWSR & 0xF8);
}

uint8_t ds3231_read_ack(void)
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  ds3231_wait();
  return TWDR;
}

uint8_t ds3231_read_nack(void)
{
  TWCR = (1 << TWINT) | (1 << TWEN);
  ds3231_wait();
  return TWDR;
}

void ds3231_read_time(ds3231_time_t *time)
{
  ds3231_start();
  ds3231_write(DS3231_ADDR_W);
  ds3231_write(0x00);

  ds3231_start();
  ds3231_write(DS3231_ADDR_R);

  time->sec = ds3231_read_ack();
  time->min = ds3231_read_ack();
  time->hour = ds3231_read_ack();
  time->day_of_week = ds3231_read_ack();
  time->day_of_month = ds3231_read_ack();
  time->month = ds3231_read_ack();
  time->year = ds3231_read_nack();

  ds3231_stop();

  time->sec = bcd_to_dec(time->sec);
  time->min = bcd_to_dec(time->min);
  time->hour = bcd_to_dec(time->hour);
  time->day_of_week = bcd_to_dec(time->day_of_week);
  time->day_of_month = bcd_to_dec(time->day_of_month);
  time->month = bcd_to_dec(time->month & 0x7F);
  time->year = bcd_to_dec(time->year);
}

uint32_t ds3231_read_time_ms(void)
{
  ds3231_time_t time;
  ds3231_read_time(&time);
  return (uint32_t)(time.hour * 3600000 + time.min * 60000 + time.sec * 1000);
}

void ds3231_set_time(const ds3231_time_t *time)
{
  ds3231_start();
  ds3231_write(DS3231_ADDR_W);
  ds3231_write(0x00);

  ds3231_write(dec_to_bcd(time->sec));
  ds3231_write(dec_to_bcd(time->min));
  ds3231_write(dec_to_bcd(time->hour));
  ds3231_write(dec_to_bcd(time->day_of_week));
  ds3231_write(dec_to_bcd(time->day_of_month));
  ds3231_write(dec_to_bcd(time->month) & 0x7F);
  ds3231_write(dec_to_bcd(time->year));

  ds3231_stop();
}

void ds3231_print_time(const ds3231_time_t *time)
{
  uart_print_int(time->day_of_month);
  uart_print("/");
  uart_print_int(time->month);
  uart_print("/");
  uart_print_int(time->year);
  uart_print(" ");
  uart_print_int(time->hour);
  uart_print(":");
  uart_print_int(time->min);
  uart_print(":");
  uart_print_int(time->sec);
}

uint8_t dec_to_bcd(uint8_t dec)
{
  return ((dec / 10) << 4) | (dec % 10);
}

uint8_t bcd_to_dec(uint8_t bcd)
{
  return (bcd & 0x0F) + ((bcd >> 4) * 10);
}
