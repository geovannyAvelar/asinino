#include "networking.h"

void networking_init(void) {
  uint8_t mac_address[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01};
  uint8_t ip_address[4] = {192, 168, 15, 100};
  uint8_t gateway_address[4] = {192, 168, 15, 1};
  uint8_t subnet_mask[4] = {255, 255, 255, 0};

  w5100_init(mac_address, ip_address, gateway_address, subnet_mask);
}