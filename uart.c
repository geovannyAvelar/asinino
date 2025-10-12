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

void uart_print_hex(unsigned int num)
{
  const char hex_chars[] = "0123456789ABCDEF";
  uart_print("0x");
  for (int i = (sizeof(num) * 2) - 1; i >= 0; i--)
  {
    uart_transmit(hex_chars[(num >> (i * 4)) & 0x0F]);
  }
}

void uart_print_int(int num)
{
  char buffer[12]; // Enough for -2147483648 and null terminator
  int is_negative = 0;
  int i = 0;

  if (num < 0)
  {
    is_negative = 1;
    num = -num;
  }

  do
  {
    buffer[i++] = (num % 10) + '0';
    num /= 10;
  } while (num > 0);

  if (is_negative)
  {
    buffer[i++] = '-';
  }

  buffer[i] = '\0';

  // Reverse the string
  for (int j = 0; j < i / 2; j++)
  {
    char temp = buffer[j];
    buffer[j] = buffer[i - j - 1];
    buffer[i - j - 1] = temp;
  }

  uart_print(buffer);
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

    if (c == 27) {
      if (uart_receive() == '[') {
        char arrow = uart_receive();
        if (arrow == 'A') {
          continue;;
        }
      }
      continue;
    }

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