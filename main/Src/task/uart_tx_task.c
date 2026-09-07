#include "task/uart_tx_task.h"
#include "common.h"
#include "freertos/idf_additions.h"
#include "uart_util.h"
#include <string.h>

#define UART_PORT UART_NUM_1

extern QueueHandle_t UART_TX_QUEUE_HANDLER;

uint8_t uart_tx_message_buffer[UART_TX_MSG_LEN_MAX];

void UAL_UART_TX_TASK_Start(void *pvParameters) {
	UART_TX_QUEUE_MSG_t queue_msg;
	TickType_t get_msg_delay = pdMS_TO_TICKS(10000);

	while(1) {
		if (xQueueReceive(UART_TX_QUEUE_HANDLER, &queue_msg, get_msg_delay)) {
			sprintf((char*) uart_tx_message_buffer, "%s", queue_msg.msg);
			uint16_t len = strlen((char*) uart_tx_message_buffer) + 1;
			UAL_STATUS_t uart_status = UAL_UART_UTIL_Transmit(UART_PORT, uart_tx_message_buffer, len);
			
			if (uart_status != UAL_STATUS_OK) {
				UAL_Error_Handler();
			}
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
