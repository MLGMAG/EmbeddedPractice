#ifndef __UART_UTIL_H
#define __UART_UTIL_H

#include "common.h"
#include <stdint.h>

#define UAL_UART_UTIL_UART_PORT UART_NUM_1

void UAL_UART_UTIL_Init();
UAL_STATUS_t UAL_UART_UTIL_Transmit(const uint8_t *data, uint16_t len);
uint16_t UAL_UART_UTIL_Receive(const uint8_t *buffer, uint16_t buffer_size, uint32_t delay);

#endif /* __UART_UTIL_H */
