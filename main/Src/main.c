#include "main.h"
#include "button_task.h"
#include "common.h"
#include "esp_log.h"
#include "freertos/FreeRTOSConfig_arch.h"
#include "freertos/idf_additions.h"
#include "sdkconfig.h"
#include "uart_rx_task.h"

TaskHandle_t BUTTON_TASK_HANDLER;
TaskHandle_t UART_RX_TASK_HANDLER;

static const char *TAG = "MAIN";

static void init() {
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
}

void app_main(void) {
	init();

	while (1) {
		vTaskDelay(portMAX_DELAY);
	}
}
