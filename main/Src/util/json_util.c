#include "util/json_util.h"
#include "esp_log.h"
#include <stdio.h>

#define JSON_DATA_FORMAT_WRITE "{\"timestamp\":\"%d-%02d-%02d %02d:%02d:%02d\",\"data\":{\"temperature\":%.2f,\"humidity\":%d,\"pressure\":%d}}"
#define JSON_DATA_FORMAT_READ "{\"timestamp\":\"%d-%02d-%02d %02d:%02d:%02d\",\"data\":{\"temperature\":%f,\"humidity\":%d,\"pressure\":%d}}"

static const char *TAG = "JSON_UTIL";

void UAL_JSON_UTIL_ConvertSensorsDataToJson(char *buffer, const SENSORS_DATA_t *sensors_data) {
	sprintf(buffer, JSON_DATA_FORMAT_WRITE,
			sensors_data->date.year, sensors_data->date.month,
			sensors_data->date.day, sensors_data->time.hour,
			sensors_data->time.minute, sensors_data->time.second,
			sensors_data->weather_data.temp,
			sensors_data->weather_data.humidity,
			sensors_data->weather_data.pressure);
}

UAL_STATUS_t UAL_JSON_UTIL_ConvertJsonToSensorsData(const char *buffer, SENSORS_DATA_t *sensors_data) {
	UAL_STATUS_t status = UAL_STATUS_OK;

	int parsed_count = sscanf(
			(char*) buffer, 
			JSON_DATA_FORMAT_READ, 
			(int *) &sensors_data->date.year,
			(int *) &sensors_data->date.month,
			(int *) &sensors_data->date.day,
			(int *) &sensors_data->time.hour,
			(int *) &sensors_data->time.minute,
			(int *) &sensors_data->time.second,
			&sensors_data->weather_data.temp,
			(int *) &sensors_data->weather_data.humidity,
			(int *) &sensors_data->weather_data.pressure
		);
		
	if (parsed_count != 9) {
		ESP_LOGE(TAG, "Failed to parse sensors data.");
		status = UAL_STATUS_ERROR;
	}
	
	return status;
}
