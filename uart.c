#include "uart.h"
#include <avr/io.h>
#include <util/delay.h>

void uart_init(unsigned int baud)
{
  UBRR0H = (unsigned char)(baud >> 8);
  UBRR0L = (unsigned char)baud;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(unsigned char data)
{
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = data;
}

void uart_print(const char *str)
{
  while (*str)
  {
    uart_transmit(*str++);
  }
}

unsigned char uart_receive(void)
{
  while (!(UCSR0A & (1 << RXC0))) {
    _delay_ms(10);
  }
  return UDR0;
}

char * uart_read_line(void) {
  static char buffer[128];
  char *ptr = buffer;
  char c;

  while (1) {
    c = uart_receive();
    if (c == '\r' || c == '\n') {
      *ptr = '\0';
      uart_print("\r\n");
      break;
    } else if (c == '\b' || c == 127) {
      if (ptr > buffer) {
        ptr--;
        uart_print("\b \b"); 
      }
    } else if (ptr - buffer < sizeof(buffer) - 1) {
      *ptr++ = c;
      uart_transmit(c);
    }
  }
  return buffer;
}