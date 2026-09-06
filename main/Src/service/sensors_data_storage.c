#include "service/sensors_data_storage.h"
#include <string.h>

static SENSORS_DATA_t sensor_data = {
	.time.hour = 0,
	.time.minute = 0,
	.time.second = 0,
	.date.year = 0,
	.date.month = 0,
	.date.day = 0,
	.weather_data.humidity = 0,
	.weather_data.pressure = 0,
	.weather_data.temp = 0
};

void UAL_SENSORS_DATA_STORAGE_Set(const SENSORS_DATA_t *data) {
	memcpy(&sensor_data, data, sizeof(SENSORS_DATA_t));
}

SENSORS_DATA_t UAL_SENSORS_DATA_STORAGE_Get() {
	SENSORS_DATA_t data = {};
	memcpy(&data, &sensor_data, sizeof(SENSORS_DATA_t));
	return data;
}
