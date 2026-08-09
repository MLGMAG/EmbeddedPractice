#include "main.h"
#include "button_task.h"
#include "common.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

TaskHandle_t BUTTON_TASK_HANDLER;

static const char *TAG = "MAIN";

static void init() {
	BaseType_t status =
		xTaskCreate(UAL_BUTTON_TASK_Start, "BUTTON_TASK", 2048, NULL,
					tskIDLE_PRIORITY + 2, &BUTTON_TASK_HANDLER);

	configASSERT(BUTTON_TASK_HANDLER);

	if (status != pdPASS) {
		ESP_LOGE(TAG, "Could not crate task, status: %d", status);
		UAL_Error_Handler();
	}
}

void app_main(void) {
	init();

	while (1) {
		vTaskDelay(portMAX_DELAY);
	}
}
