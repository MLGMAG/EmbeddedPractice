#include "service/sensors_data_storage.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include <string.h>

static const char *TAG = "SENSORS_DATA_STORAGE";

extern SemaphoreHandle_t SENSORS_DATA_STORAGE_MUTEX;

static SENSORS_DATA_t sensor_data = {.time.hour = 0,
									 .time.minute = 0,
									 .time.second = 0,
									 .date.year = 0,
									 .date.month = 0,
									 .date.day = 0,
									 .weather_data.humidity = 0,
									 .weather_data.pressure = 0,
									 .weather_data.temp = 0};

void UAL_SENSORS_DATA_STORAGE_Set(const SENSORS_DATA_t *data) {
	BaseType_t status =
		xSemaphoreTake(SENSORS_DATA_STORAGE_MUTEX, pdMS_TO_TICKS(1000));
	if (status == pdTRUE) {
		memcpy(&sensor_data, data, sizeof(SENSORS_DATA_t));
		xSemaphoreGive(SENSORS_DATA_STORAGE_MUTEX);
	} else {
		ESP_LOGW(TAG, "Could not acquire mutex on set", status);
	}
}

SENSORS_DATA_t UAL_SENSORS_DATA_STORAGE_Get() {
	SENSORS_DATA_t data = {};

	BaseType_t status =
		xSemaphoreTake(SENSORS_DATA_STORAGE_MUTEX, pdMS_TO_TICKS(1000));
	if (status == pdTRUE) {
		memcpy(&data, &sensor_data, sizeof(SENSORS_DATA_t));
		xSemaphoreGive(SENSORS_DATA_STORAGE_MUTEX);
	} else {
		ESP_LOGW(TAG, "Could not acquire mutex on get", status);
	}

	return data;
}
