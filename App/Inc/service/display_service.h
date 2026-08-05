#ifndef __DISPLAY_SERVICE_H
#define __DISPLAY_SERVICE_H

#include "common.h"

void UAL_DISPLAY_SERVICE_Init();
void UAL_DISPLAY_SERVICE_Screen_Enable();
void UAL_DISPLAY_SERVICE_Screen_Disable();
void UAL_DISPLAY_SERVICE_SendorData_FillTitles();
void UAL_DISPLAY_SERVICE_SensorData_Update(SENSORS_DATA_t *data);
void UAL_DISPLAY_SERVICE_DateField_Update(DATE_t* date);
void UAL_DISPLAY_SERVICE_TimeField_Update(TIME_t* time);
void UAL_DISPLAY_SERVICE_TemperatureField_Update(float temperature);
void UAL_DISPLAY_SERVICE_HumidityField_Update(uint16_t humidity);
void UAL_DISPLAY_SERVICE_PressureField_Update(uint16_t pressure);

#endif /* __DISPLAY_SERVICE_H */
