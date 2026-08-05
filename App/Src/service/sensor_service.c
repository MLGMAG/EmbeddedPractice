#include "service/sensor_service.h"
#include "bme280.h"
#include "ds1307.h"
#include "cmsis_os.h"

#define TIME_ZONE 2

void UAL_SENSOR_SERVICE_BME280_Init(void) {
	BME280_Init_t BME280_InitStruct = { 0 };

	BME280_Reset();

	BME280_InitStruct.Filter = FILTER_8;
	BME280_InitStruct.Mode = BME280_NORMAL_MODE;
	BME280_InitStruct.OverSampling_H = OVERSAMPLING_16;
	BME280_InitStruct.OverSampling_P = OVERSAMPLING_16;
	BME280_InitStruct.OverSampling_T = OVERSAMPLING_16;
	BME280_InitStruct.SPI_EnOrDıs = SPI3_W_DISABLE;
	BME280_InitStruct.T_StandBy = T_SB_250;

	BME280_Init(BME280_InitStruct);
}

void UAL_SENSOR_SERVICE_DS1307_Init(void) {
	DS1307_Init();

	DS1307_SetTimeZone(TIME_ZONE, 00);

	if (DS1307_GetYear() > 10000) {
		DS1307_SetYear(0);
	}
}

SENSORS_DATA_t UAL_SENSOR_SERVICE_GetData() {
	SENSORS_DATA_t data;

	BME280_Data_t BME280;
	BME280_Calculation(&BME280);
	data.weather_data.humidity = BME280.Humidity;
	data.weather_data.pressure = BME280.Pressure;
	data.weather_data.temp = BME280.Temperature;

	data.date.year = DS1307_GetYear();
	data.date.month = DS1307_GetMonth();
	data.date.day = DS1307_GetDate();
	data.time.hour = DS1307_GetHour();
	data.time.minute = DS1307_GetMinute();
	data.time.second = DS1307_GetSecond();

	return data;
}

void UAL_SENSOR_SERVICE_SetDate(const DATE_t* date) {
	DS1307_SetDate(date->day);
	DS1307_SetMonth(date->month);
	DS1307_SetYear(date->year);
}

void UAL_SENSOR_SERVICE_SetTime(const TIME_t* time) {
	DS1307_SetHour(time->hour);
	DS1307_SetMinute(time->minute);
	DS1307_SetSecond(time->second);
}
