#include "hardware/uart_util.h"
#include "stm32f4xx_hal.h"
#include "string.h"

#define TIMEOUT_MS 100
#define UART_RX_MSG_LEN_MAX 32

extern UART_HandleTypeDef huart1;

static uint8_t uart_dma_buffer[UART_RX_MSG_LEN_MAX];
static uint8_t uart_rx_message_buffer[UART_RX_MSG_LEN_MAX];
static volatile uint16_t last_rx_offset = 0;
static volatile uint16_t uart_rx_available = 0;
static volatile uint16_t dma_buffer_len = 0;

static void UART_DMA_ProcessChar(char c) {
	if (c == '\n') {
		uart_rx_available = 1;
		HAL_UART_DMAStop(&huart1);
	} else {
		if (dma_buffer_len < UART_RX_MSG_LEN_MAX) {
			strncat((char*) &uart_rx_message_buffer, &c, 1);
			dma_buffer_len++;
		} else {
			uart_rx_message_buffer[0] = '\0';
			dma_buffer_len = 0;
		}
	}
}

UAL_STATUS_t UAL_UART_UTIL_Transmit(const uint8_t *data, uint16_t len) {
	HAL_StatusTypeDef uart_status = HAL_UART_Transmit(&huart1, data, len,
	TIMEOUT_MS);

	if (uart_status != HAL_OK) {
		return UAL_STATUS_ERROR;
	}

	return UAL_STATUS_OK;
}

void UAL_UART_UTIL_Rx_EnableDma() {
	uart_rx_message_buffer[0] = '\0';
	dma_buffer_len = 0;

	HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_DMA(
		&huart1,
		uart_dma_buffer,
		UART_RX_MSG_LEN_MAX
	);

	if (status != HAL_OK) {
		UAL_Error_Handler();
	}
}

uint8_t UAL_UART_UTIL_Rx_Available() {
	return uart_rx_available;
}

void UAL_UART_UTIL_Rx_Reset() {
	uart_rx_available = 0;
}

uint8_t* UAL_UART_UTIL_Rx_GetBuffer() {
	return uart_rx_message_buffer;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t offset) {
	if (offset < last_rx_offset) {
		last_rx_offset = 0;
	}

	while (last_rx_offset < offset) {
		UART_DMA_ProcessChar((char) uart_dma_buffer[last_rx_offset]);
		last_rx_offset++;
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
//	UAL_Error_Handler();
	UAL_UART_UTIL_Rx_EnableDma();
}
