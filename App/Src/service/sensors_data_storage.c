#include "service/sensors_data_storage.h"
#include "hardware/flash_util.h"
#include "string.h"

#define FLASH_SIZE UAL_SENSORS_DATA_STORAGE_MAX_SIZE / UAL_FLASH_UTIL_DATA_SIZE

static SENSORS_DATA_t sensor_data_logs[UAL_SENSORS_DATA_STORAGE_MAX_SIZE];
static uint16_t sensor_data_logs_index = 0;
static uint8_t is_enabled = 0;

void UAL_SENSORS_DATA_STORAGE_Collect(const SENSORS_DATA_t *data) {
	if (is_enabled) {
		SENSORS_DATA_t *sensors_data = &sensor_data_logs[sensor_data_logs_index];
		memcpy(sensors_data, data, sizeof(SENSORS_DATA_t));
		sensor_data_logs_index++;
		if (sensor_data_logs_index == UAL_SENSORS_DATA_STORAGE_MAX_SIZE) {
			sensor_data_logs_index = 0;
		}
	}
}

void UAL_SENSORS_DATA_STORAGE_PeristToFlash() {
	if (is_enabled) {
		UAL_FLASH_UTIL_Write(
			(uint32_t*) sensor_data_logs,
			FLASH_SIZE,
			UAL_Error_Handler
		);
	}
}

void UAL_SENSORS_DATA_STORAGE_Enable() {
	is_enabled = 1;
}

void UAL_SENSORS_DATA_STORAGE_Disable() {
	is_enabled = 0;
}
