#ifndef __SENSOR_SERVICE_H
#define __SENSOR_SERVICE_H

#include "common.h"

void UAL_SENSOR_SERVICE_BME280_Init(void);
void UAL_SENSOR_SERVICE_DS1307_Init(void);
SENSORS_DATA_t UAL_SENSOR_SERVICE_GetData();
void UAL_SENSOR_SERVICE_SetDate(const DATE_t* date);
void UAL_SENSOR_SERVICE_SetTime(const TIME_t* time);

#endif /* __SENSOR_SERVICE_H */
