#ifndef UART_H
#define UART_H
#include "handle_cmd.h"
#include <string.h>
#include "stdint.h"
void receive_data(uint8_t c);
void uart_receive_handle(HostwebTypedef *hostweb_node2, Dht22_Typedef *dht22);
void uart_init();
#endif