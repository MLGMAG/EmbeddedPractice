#ifndef __COMMON_H
#define __COMMON_H

#include "stdint.h"

#define UART_TX_MSG_LEN_MAX 128

typedef enum {
	UAL_STATUS_OK, UAL_STATUS_ERROR,
} UAL_STATUS_t;

typedef struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
} DATE_t;

typedef struct {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} TIME_t;

typedef struct {
	uint16_t humidity;
	uint16_t pressure;
	float temp;
} WEATHER_DATA_t;


typedef struct {
	WEATHER_DATA_t weather_data;
	DATE_t date;
	TIME_t time;
} SENSORS_DATA_t;


typedef struct {
	SENSORS_DATA_t sensors_data;
} DISPLAY_MSG_t;

typedef struct {
	char msg[UART_TX_MSG_LEN_MAX];
} UART_TX_QUEUE_MSG_t;

void UAL_QueueTimeoutError_Handler(void);
void UAL_Error_Handler(void);

#endif /* __COMMON_H */
