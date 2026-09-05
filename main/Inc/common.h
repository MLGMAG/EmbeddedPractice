#ifndef __COMMON_H
#define __COMMON_H

#define UART_TX_MSG_LEN_MAX 128

typedef enum { GPIO_PIN_RESET = 0, GPIO_PIN_SET } GPIO_PinState;

typedef enum {
	UAL_STATUS_OK,
	UAL_STATUS_ERROR,
} UAL_STATUS_t;

typedef struct {
	char msg[UART_TX_MSG_LEN_MAX];
} UART_TX_QUEUE_MSG_t;

void UAL_QueueTimeoutError_Handler(void);

void UAL_Error_Handler(void);

#endif /* __COMMON_H */
