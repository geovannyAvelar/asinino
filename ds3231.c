#include "ds3231.h"
#include "uart.h"
#include <avr/io.h>
#include <util/delay.h>

void ds3231_init(void)
{
  TWSR = 0x00;
  // Set bit rate register (see datasheet for formula)
  TWBR = (F_CPU / SCL_CLOCK - 16) / 2;
  // Enable TWI
  TWCR = (1 << TWEN);
}

// Wait for TWI interrupt flag to be set
void ds3231_wait(void)
{
  while (!(TWCR & (1 << TWINT))) {
    _delay_ms(100);
  }
}

// Send START condition
void ds3231_start(void)
{
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  ds3231_wait();
}

// Send STOP condition
void ds3231_stop(void)
{
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// Transmit a byte
uint8_t ds3231_write(uint8_t data)
{
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  ds3231_wait();
  return (TWSR & 0xF8); // Return status code
}

// Receive a byte with ACK (acknowledge more data)
uint8_t ds3231_read_ack(void)
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  ds3231_wait();
  return TWDR;
}

// Receive a byte with NACK (no more data)
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
  uart_print("\r\n");
}

uint8_t dec_to_bcd(uint8_t dec)
{
  return ((dec / 10) << 4) | (dec % 10);
}

uint8_t bcd_to_dec(uint8_t bcd)
{
  return (bcd & 0x0F) + ((bcd >> 4) * 10);
}
