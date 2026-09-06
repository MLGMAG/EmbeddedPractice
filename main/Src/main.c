#include "main.h"
#include "common.h"
#include "esp_log.h"
#include "freertos/FreeRTOSConfig_arch.h"
#include "freertos/idf_additions.h"
#include "sdkconfig.h"
#include "task/button_task.h"
#include "task/uart_rx_task.h"
#include "task/uart_tx_task.h"
#include "task/command_handler_task.h"
#include "hardware/uart_util.h"

TaskHandle_t BUTTON_TASK_HANDLER;
TaskHandle_t UART_RX_TASK_HANDLER;
TaskHandle_t UART_TX_TASK_HANDLER;
TaskHandle_t COMMAND_HANDLER_TASK_HANDLER;

QueueHandle_t UART_TX_QUEUE_HANDLER;
QueueHandle_t COMMAND_QUEUE_HANDLER;


static const char *TAG = "MAIN";


static void init_tasks(void) {
	BaseType_t status;

	status = xTaskCreate(UAL_BUTTON_TASK_Start, "BUTTON_TASK",
						 configMINIMAL_STACK_SIZE, NULL,
						 CONFIG_BUTTON_TASK_PRIORITY, &BUTTON_TASK_HANDLER);
	configASSERT(BUTTON_TASK_HANDLER);
	if (status != pdPASS) {
		ESP_LOGE(TAG, "Could not create BUTTON_TASK task, status: %d", status);
		UAL_Error_Handler();
	}

	status = xTaskCreate(UAL_UART_RX_TASK_Start, "UART_RX_TASK",
						 configMINIMAL_STACK_SIZE, NULL,
						 CONFIG_UART_RX_TASK_PRIORITY, &UART_RX_TASK_HANDLER);
	configASSERT(UART_RX_TASK_HANDLER);
	if (status != pdPASS) {
		ESP_LOGE(TAG, "Could not create UART_RX_TASK task, status: %d", status);
		UAL_Error_Handler();
	}
	
	status = xTaskCreate(UAL_UART_TX_TASK_Start, "UART_TX_TASK",
						 configMINIMAL_STACK_SIZE, NULL,
						 CONFIG_UART_TX_TASK_PRIORITY, &UART_TX_TASK_HANDLER);
	configASSERT(UART_TX_TASK_HANDLER);
	if (status != pdPASS) {
		ESP_LOGE(TAG, "Could not create UART_RX_TASK task, status: %d", status);
		UAL_Error_Handler();
	}
	
	status = xTaskCreate(UAL_COMMAND_HANDLER_TASK_Start, "COMMAND_HANDLER_TASK",
						 configMINIMAL_STACK_SIZE, NULL,
						 CONFIG_COMMAND_HANDLER_TASK_PRIORITY, &COMMAND_HANDLER_TASK_HANDLER);
	 if (status != pdPASS) {
	 	ESP_LOGE(TAG, "Could not create COMMAND_HANDLER_TASK task, status: %d", status);
	 	UAL_Error_Handler();
	 }
}

static void init(void) {
	UART_TX_QUEUE_HANDLER = xQueueCreate(5, sizeof(UART_TX_QUEUE_MSG_t));
	COMMAND_QUEUE_HANDLER = xQueueCreate(5, sizeof(COMMAND_QUEUE_MSG_t));

	UAL_UART_UTIL_Init();

	init_tasks();
}

void app_main(void) {
	init();

	while (1) {
		vTaskDelay(portMAX_DELAY);
	}
}
