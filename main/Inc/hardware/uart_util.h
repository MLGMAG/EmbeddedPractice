#ifndef __UART_UTIL_H
#define __UART_UTIL_H

#include "common.h"
#include "freertos/idf_additions.h"
#include "hal/uart_types.h"
#include <stdint.h>

void UAL_UART_UTIL_Init();
uint8_t UAL_UART_UTIL_QueueIsEmpty(QueueHandle_t queue, uint32_t delay);
UAL_STATUS_t UAL_UART_UTIL_Transmit(uart_port_t port, const uint8_t *data, uint16_t len);
uint16_t UAL_UART_UTIL_Receive(uart_port_t port, QueueHandle_t queue, const uint8_t *buffer, uint16_t buffer_size, uint32_t delay);

#endif /* __UART_UTIL_H */
