#include "networking.h"
#include "../uart.h"
#include "../log.h"

void networking_init(void) {
  info("Initializing networking...");

  spi_init();

  uint8_t mac_address[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01};
  uint8_t ip_address[4] = {192, 168, 15, 100};
  uint8_t gateway_address[4] = {192, 168, 15, 1};
  uint8_t subnet_mask[4] = {255, 255, 255, 0};

  w5100_init(mac_address, ip_address, gateway_address, subnet_mask);

  info("Networking initialized.");
  uart_print("MAC Address: ");
  for (int i = 0; i < 6; i++) {
    uart_print_hex_f(mac_address[i]);
    if (i < 5) uart_print(":");
  }
  uart_print("\r\n");

  uart_print("IP Address: ");
  for (int i = 0; i < 4; i++) {
    uart_print_int(ip_address[i]);
    if (i < 3) uart_print(".");
  }
  uart_print("\r\n");

  uart_print("Gateway Address: ");
  for (int i = 0; i < 4; i++) {
    uart_print_int(gateway_address[i]);
    if (i < 3) uart_print(".");
  }
  uart_print("\r\n");

  uart_print("Subnet Mask: ");
  for (int i = 0; i < 4; i++) {
    uart_print_int(subnet_mask[i]);
    if (i < 3) uart_print(".");
  }
  uart_print("\r\n");
}