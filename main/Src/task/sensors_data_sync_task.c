#include "task/sensors_data_sync_task.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

static const char *TAG = "SENSORS_DATA_SYNC_TASK";

void UAL_SENSORS_DATA_SYNC_TASK_Start(void *pvParameters) {
	while (1) {
		ESP_LOGI(TAG, "Synchronize sensors data.");
		vTaskDelay(pdMS_TO_TICKS(60000));
	}
}
