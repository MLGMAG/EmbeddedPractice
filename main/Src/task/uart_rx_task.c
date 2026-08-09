#include "uart_rx_task.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"

static const char *TAG = "UART_RX_TASK";

void UAL_UART_RX_TASK_Start(void *pvParameters) {
	while(1) {
		ESP_LOGI(TAG, "Timeout...");
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}
