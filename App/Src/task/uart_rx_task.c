#include "task/uart_rx_task.h"
#include "cmsis_os.h"
#include "hardware/uart_util.h"
#include "service/command_handler.h"

void UAL_UART_RX_TASK_Start(void *argument) {
	UAL_UART_UTIL_Rx_EnableDma();

	while (1) {
		if (UAL_UART_UTIL_Rx_Available()) {
			uint8_t* message = UAL_UART_UTIL_Rx_GetBuffer();
			UAL_COMMAND_HANDLER_Handle(message);

			UAL_UART_UTIL_Rx_EnableDma();
			UAL_UART_UTIL_Rx_Reset();
		}
		osDelay(pdMS_TO_TICKS(100));
	}
}
