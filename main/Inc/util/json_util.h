#ifndef __JSON_UTIL_H
#define __JSON_UTIL_H

#include "common.h"

void UAL_JSON_UTIL_ConvertSensorsDataToJson(char *buffer, const SENSORS_DATA_t *sensors_data);
UAL_STATUS_t UAL_JSON_UTIL_ConvertJsonToSensorsData(const char *buffer, SENSORS_DATA_t *sensors_data);

#endif /* __JSON_UTIL_H */
