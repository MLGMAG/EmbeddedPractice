#include "service/command_handler.h"
#include "cmsis_os.h"
#include "common.h"
#include "common_rtos.h"
#include "stdio.h"
#include "string.h"
#include "service/sensors_data_storage.h"
#include "service/sensor_service.h"

#define STATUS_COMMAND "status"
#define LOG_ON_COMMAND "log on"
#define LOG_OFF_COMMAND "log off"
#define FLUSH_COMMAND "flush"
#define SET_DATE_COMMAND "set-date"
#define SET_TIME_COMMAND "set-time"

#define JSON_DATA_FORMAT "{\"timestamp\":\"%d-%02d-%02d %02d:%02d:%02d\",\"data\":{\"temperature\":%.2f,\"humidity\":%d,\"pressure\":%d}}\n"

extern osMessageQueueId_t uart_tx_queueHandle;

static uint8_t is_date_valid(DATE_t* date) {
	if (date->day > 31 || date->day == 0) {
		return 0;
	}

	if (date->month > 12 || date->month == 0) {
		return 0;
	}

	if (date->year == 0) {
		return 0;
	}

	return 1;
}

static uint8_t is_time_valid(TIME_t* time) {
	if (time->hour > 23) {
		return 0;
	}

	if (time->minute > 59) {
		return 0;
	}

	if (time->second > 59) {
		return 0;
	}

	return 1;
}

void UAL_COMMAND_HANDLER_Handle(uint8_t* command) {
	if (strcmp(STATUS_COMMAND, (char*) command) == 0) {
		SENSORS_DATA_t sensors_data = UAL_SENSOR_SERVICE_GetData();

		UART_TX_QUEUE_MSG_t uart_queue_msg;
		sprintf((char*) uart_queue_msg.msg, JSON_DATA_FORMAT,
				sensors_data.date.year, sensors_data.date.month,
				sensors_data.date.day, sensors_data.time.hour,
				sensors_data.time.minute, sensors_data.time.second,
				sensors_data.weather_data.temp,
				sensors_data.weather_data.humidity,
				sensors_data.weather_data.pressure);
		osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
				pdMS_TO_TICKS(1000));
		UAL_MessageQueue_CheckStatus(&status);
	}

	if (strcmp(LOG_ON_COMMAND, (char*) command) == 0) {
		UAL_SENSORS_DATA_STORAGE_Enable();
		UART_TX_QUEUE_MSG_t uart_queue_msg;
		sprintf((char*) uart_queue_msg.msg, "Logging is enabled.\n");
		osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
				pdMS_TO_TICKS(1000));
		UAL_MessageQueue_CheckStatus(&status);
	}

	if (strcmp(LOG_OFF_COMMAND, (char*) command) == 0) {
		UAL_SENSORS_DATA_STORAGE_Disable();
		UART_TX_QUEUE_MSG_t uart_queue_msg;
		sprintf((char*) uart_queue_msg.msg, "Logging is disabled.\n");
		osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
				pdMS_TO_TICKS(1000));
		UAL_MessageQueue_CheckStatus(&status);
	}

	if (strcmp(FLUSH_COMMAND, (char*) command) == 0) {
		UAL_SENSORS_DATA_STORAGE_PeristToFlash();
		UART_TX_QUEUE_MSG_t uart_queue_msg;
		sprintf((char*) uart_queue_msg.msg, "Flush is done.\n");
		osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
				pdMS_TO_TICKS(1000));
		UAL_MessageQueue_CheckStatus(&status);
	}

	if (strncmp(SET_DATE_COMMAND, (char*) command, strlen(SET_DATE_COMMAND)) == 0) {
		UART_TX_QUEUE_MSG_t uart_queue_msg;
		DATE_t date;

		int parsed_count = sscanf((char*) command, "set-date %d-%d-%d", &date.year, &date.month, &date.day);

		if (parsed_count != 3) {
			sprintf((char*) uart_queue_msg.msg, "Could not parse date.\n");
			osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
					pdMS_TO_TICKS(1000));
			UAL_MessageQueue_CheckStatus(&status);
		} else if (!is_date_valid(&date)) {
			sprintf((char*) uart_queue_msg.msg, "Input date is invalid.\n");
			osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
					pdMS_TO_TICKS(1000));
			UAL_MessageQueue_CheckStatus(&status);
		} else {
			UAL_SENSOR_SERVICE_SetDate(&date);
			sprintf((char*) uart_queue_msg.msg, "Date is set.\n");
			osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
					pdMS_TO_TICKS(1000));
			UAL_MessageQueue_CheckStatus(&status);
		}
	}

	if (strncmp(SET_TIME_COMMAND, (char*) command, strlen(SET_TIME_COMMAND)) == 0) {
		UART_TX_QUEUE_MSG_t uart_queue_msg;
		TIME_t time;

	    int parsed_count = sscanf((char*) command, "set-time %d:%d:%d", &time.hour, &time.minute, &time.second);

		if (parsed_count != 3) {
			sprintf((char*) uart_queue_msg.msg, "Could not parse time.\n");
			osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
					pdMS_TO_TICKS(1000));
			UAL_MessageQueue_CheckStatus(&status);
		} else if (!is_time_valid(&time)) {
			sprintf((char*) uart_queue_msg.msg, "Input time is invalid.\n");
			osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
					pdMS_TO_TICKS(1000));
			UAL_MessageQueue_CheckStatus(&status);
		} else {
			UAL_SENSOR_SERVICE_SetTime(&time);
			sprintf((char*) uart_queue_msg.msg, "Time is set.\n");
			osStatus_t status = osMessageQueuePut(uart_tx_queueHandle, &uart_queue_msg, 0U,
					pdMS_TO_TICKS(1000));
			UAL_MessageQueue_CheckStatus(&status);
		}
	}
}
