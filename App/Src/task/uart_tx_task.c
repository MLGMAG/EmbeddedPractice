#include "task/uart_tx_task.h"
#include "cmsis_os.h"
#include "common.h"
#include "stdio.h"
#include "string.h"
#include "hardware/uart_util.h"

extern osMessageQueueId_t uart_tx_queueHandle;

uint8_t uart_tx_message_buffer[UART_TX_MSG_LEN_MAX];

void UAL_UART_TX_TASK_Start(void *argument) {
	UART_TX_QUEUE_MSG_t queue_msg;
	osStatus_t status;
	uint32_t getMessage_delay = pdMS_TO_TICKS(10000);

	while (1) {
		status = osMessageQueueGet(uart_tx_queueHandle, &queue_msg, NULL,
				getMessage_delay);

		if (status == osOK) {
			sprintf((char*) uart_tx_message_buffer, "%s", queue_msg.msg);
			uint16_t len = strlen((char*) uart_tx_message_buffer) + 1;

			UAL_STATUS_t uart_status = UAL_UART_UTIL_Transmit(uart_tx_message_buffer, len);

			if (uart_status != UAL_STATUS_OK) {
				UAL_Error_Handler();
			}
		} else if (status != osErrorTimeout) {
			UAL_Error_Handler();
		}

		osDelay(1);
	}
}
