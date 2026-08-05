#ifndef __UART_UTIL_H
#define __UART_UTIL_H

#include "stdint.h"
#include "common.h"

UAL_STATUS_t UAL_UART_UTIL_Transmit(const uint8_t *data, uint16_t len);
void UAL_UART_UTIL_Rx_EnableDma();
uint8_t UAL_UART_UTIL_Rx_Available();
void UAL_UART_UTIL_Rx_Reset();
uint8_t* UAL_UART_UTIL_Rx_GetBuffer();

#endif /* __UART_UTIL_H */
