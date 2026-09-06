#include "util/json_util.h"
#include <stdio.h>

#define JSON_DATA_FORMAT "{\"timestamp\":\"%d-%02d-%02d %02d:%02d:%02d\",\"data\":{\"temperature\":%.2f,\"humidity\":%d,\"pressure\":%d}}"

void UAL_JSON_UTIL_SensorsDataCovertToJson(char *buffer, const SENSORS_DATA_t *sensors_data) {
	sprintf(buffer, JSON_DATA_FORMAT,
			sensors_data->date.year, sensors_data->date.month,
			sensors_data->date.day, sensors_data->time.hour,
			sensors_data->time.minute, sensors_data->time.second,
			sensors_data->weather_data.temp,
			sensors_data->weather_data.humidity,
			sensors_data->weather_data.pressure);
}
