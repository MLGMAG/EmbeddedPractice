#include "service/display_service.h"
#include "fonts/font_10_roboto_thin.h"
#include "ssd1331.h"
#include "stdio.h"

static uint8_t screen_message_buffer[16];

#define DATE_TITLE_POSITION_X 5
#define DATE_TITLE_POSITION_Y 8

#define DATE_VALUE_POSITION_X0 38
#define DATE_VALUE_POSITION_X1 100
#define DATE_VALUE_POSITION_Y0 0
#define DATE_VALUE_POSITION_Y1 8

#define TIME_TITLE_POSITION_X 5
#define TIME_TITLE_POSITION_Y 20

#define TIME_VALUE_POSITION_X0 38
#define TIME_VALUE_POSITION_X1 100
#define TIME_VALUE_POSITION_Y0 12
#define TIME_VALUE_POSITION_Y1 20

#define TEMPERATURE_TITLE_POSITION_X 5
#define TEMPERATURE_TITLE_POSITION_Y 32

#define TEMPERATURE_VALUE_POSITION_X0 16
#define TEMPERATURE_VALUE_POSITION_X1 48
#define TEMPERATURE_VALUE_POSITION_Y0 24
#define TEMPERATURE_VALUE_POSITION_Y1 32

#define RH_TITLE_POSITION_X 48
#define RH_TITLE_POSITION_Y 32

#define RH_VALUE_POSITION_X0 67
#define RH_VALUE_POSITION_X1 100
#define RH_VALUE_POSITION_Y0 24
#define RH_VALUE_POSITION_Y1 32

#define PRESSURE_TITLE_POSITION_X 5
#define PRESSURE_TITLE_POSITION_Y 44

#define PRESSURE_VALUE_POSITION_X0 16
#define PRESSURE_VALUE_POSITION_X1 70
#define PRESSURE_VALUE_POSITION_Y0 36
#define PRESSURE_VALUE_POSITION_Y1 44

void UAL_DISPLAY_SERVICE_Init() {
	SSD1331_Init();
}

void UAL_DISPLAY_SERVICE_Screen_Enable() {
	SSD1331_setDisplayMode(SSD1331_NormalDisplay);
}

void UAL_DISPLAY_SERVICE_Screen_Disable() {
	SSD1331_setDisplayMode(SSD1331_DisplayOff);
}

void UAL_DISPLAY_SERVICE_SendorData_FillTitles() {
	SSD1331_Font(
		DATE_TITLE_POSITION_X,
		DATE_TITLE_POSITION_Y,
		"Date:",
		&Roboto_Thin_10,
		1,
		SSD1331_WHITE
	);

	SSD1331_Font(
		TIME_TITLE_POSITION_X,
		TIME_TITLE_POSITION_Y,
		"Time:",
		&Roboto_Thin_10,
		1,
		SSD1331_WHITE
	);

	SSD1331_Font(
		TEMPERATURE_TITLE_POSITION_X,
		TEMPERATURE_TITLE_POSITION_Y,
		"T:",
		&Roboto_Thin_10,
		1,
		SSD1331_WHITE
	);

	SSD1331_Font(
		RH_TITLE_POSITION_X,
		RH_TITLE_POSITION_Y,
		"RH:",
		&Roboto_Thin_10,
		1,
		SSD1331_WHITE
	);

	SSD1331_Font(
		PRESSURE_TITLE_POSITION_X,
		PRESSURE_TITLE_POSITION_Y,
		"P:",
		&Roboto_Thin_10,
		1,
		SSD1331_WHITE
	);
}

void UAL_DISPLAY_SERVICE_SensorData_Update(SENSORS_DATA_t *data) {
	UAL_DISPLAY_SERVICE_DateField_Update(&data->date);
	UAL_DISPLAY_SERVICE_TimeField_Update(&data->time);
	UAL_DISPLAY_SERVICE_TemperatureField_Update(data->weather_data.temp);
	UAL_DISPLAY_SERVICE_HumidityField_Update(data->weather_data.humidity);
	UAL_DISPLAY_SERVICE_PressureField_Update(data->weather_data.pressure);
}

void UAL_DISPLAY_SERVICE_DateField_Update(DATE_t* date) {
	SSD1331_clearWindow(
		DATE_VALUE_POSITION_X0,
		DATE_VALUE_POSITION_Y0,
		DATE_VALUE_POSITION_X1,
		DATE_VALUE_POSITION_Y1
	);

	sprintf(
		(char*) screen_message_buffer,
		"%04d-%02d-%02d",
		date->year,
		date->month,
		date->day
	);

	SSD1331_Font(
		DATE_VALUE_POSITION_X0,
		DATE_VALUE_POSITION_Y1,
		(char*) screen_message_buffer,
		&Roboto_Thin_10,
		1,
		SSD1331_YELLOW
	);
}

void UAL_DISPLAY_SERVICE_TimeField_Update(TIME_t* time) {
	SSD1331_clearWindow(
		TIME_VALUE_POSITION_X0,
		TIME_VALUE_POSITION_Y0,
		TIME_VALUE_POSITION_X1,
		TIME_VALUE_POSITION_Y1
	);

	sprintf(
		(char*) screen_message_buffer,
		"%02d:%02d:%02d",
		time->hour,
		time->minute,
		time->second
	);

	SSD1331_Font(
		TIME_VALUE_POSITION_X0,
		TIME_VALUE_POSITION_Y1,
		(char*) screen_message_buffer,
		&Roboto_Thin_10,
		1,
		SSD1331_YELLOW
	);
}

void UAL_DISPLAY_SERVICE_TemperatureField_Update(float temperature) {
	SSD1331_clearWindow(
		TEMPERATURE_VALUE_POSITION_X0,
		TEMPERATURE_VALUE_POSITION_Y0,
		TEMPERATURE_VALUE_POSITION_X1,
		TEMPERATURE_VALUE_POSITION_Y1
	);

	sprintf((char*) screen_message_buffer, "%.1fC", temperature);

	SSD1331_Font(
		TEMPERATURE_VALUE_POSITION_X0,
		TEMPERATURE_VALUE_POSITION_Y1,
		(char*) screen_message_buffer,
		&Roboto_Thin_10,
		1,
		SSD1331_CYAN
	);
}

void UAL_DISPLAY_SERVICE_HumidityField_Update(uint16_t humidity) {
	SSD1331_clearWindow(
		RH_VALUE_POSITION_X0,
		RH_VALUE_POSITION_Y0,
		RH_VALUE_POSITION_X1,
		RH_VALUE_POSITION_Y1
	);

	sprintf((char*) screen_message_buffer, "%d%%", humidity);

	SSD1331_Font(
		RH_VALUE_POSITION_X0,
		RH_VALUE_POSITION_Y1,
		(char*) screen_message_buffer,
		&Roboto_Thin_10,
		1,
		SSD1331_CYAN
	);
}

void UAL_DISPLAY_SERVICE_PressureField_Update(uint16_t pressure) {
	SSD1331_clearWindow(
		PRESSURE_VALUE_POSITION_X0,
		PRESSURE_VALUE_POSITION_Y0,
		PRESSURE_VALUE_POSITION_X1,
		PRESSURE_VALUE_POSITION_Y1
	);

	sprintf((char*) screen_message_buffer, "%d hPa", pressure);

	SSD1331_Font(
		PRESSURE_VALUE_POSITION_X0,
		PRESSURE_VALUE_POSITION_Y1,
		(char*) screen_message_buffer,
		&Roboto_Thin_10,
		1,
		SSD1331_GREEN_D
	);
}
