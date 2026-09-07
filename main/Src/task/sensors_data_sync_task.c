#include "task/sensors_data_sync_task.h"
#include "common.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "hardware/uart_util.h"
#include "json_util.h"
#include "sdkconfig.h"
#include <string.h>
#include "service/sensors_data_storage.h"

#define RETRIEVE_DATA_COMMAND "status"
#define UART_PORT UART_NUM_2
#define BUFFER_LEN 128

static const char *TAG = "SENSORS_DATA_SYNC_TASK";

extern QueueHandle_t UAL_UART2_UTIL_QUEUE;

static uint8_t raw_data_buffer[BUFFER_LEN] = {0};

static UAL_STATUS_t request_data(uint8_t* buffer) {
	sprintf((char *)buffer, "%s\n", RETRIEVE_DATA_COMMAND);
	
	UAL_STATUS_t uart_status = UAL_UART_UTIL_Transmit(
		UART_PORT, 
		buffer, 
		strlen((char *)buffer)
	);
	
	if (uart_status != UAL_STATUS_OK) {
		ESP_LOGE(TAG, "Failed to request sensors data.");
	}
	
	return uart_status;
}

static UAL_STATUS_t receive_data(uint8_t* buffer, uint16_t buffer_len) {
	UAL_STATUS_t status = UAL_STATUS_OK;
	uint8_t data_pointer = 0;
	buffer[0] = '\0';

	while (!UAL_UART_UTIL_QueueIsEmpty(UAL_UART2_UTIL_QUEUE, pdMS_TO_TICKS(1000))) {
		uint16_t data_len = UAL_UART_UTIL_Receive(
			UART_PORT, 
			UAL_UART2_UTIL_QUEUE,
			(buffer + data_pointer), 
			buffer_len - data_pointer, 
			0
		);
		data_pointer += data_len;
	}

	if (buffer[0] == '\0') {
		ESP_LOGE(TAG, "Failed to receive sensors data.");
		status = UAL_STATUS_ERROR;
	} 

	if (buffer[0] != '\0') {
		raw_data_buffer[strlen((char *)raw_data_buffer) - 1] = '\0';
	}

	return status;
}

void UAL_SENSORS_DATA_SYNC_TASK_Start(void *pvParameters) {
	SENSORS_DATA_t sensors_data = {};

	while (1) {
		ESP_LOGI(TAG, "Synchronize sensors data.");

		UAL_STATUS_t status = request_data(raw_data_buffer);

		if (status == UAL_STATUS_OK) {
			status = receive_data(raw_data_buffer, BUFFER_LEN);
		}
		
		if (status == UAL_STATUS_OK) {
			status = UAL_JSON_UTIL_ConvertJsonToSensorsData(
				(char *) raw_data_buffer, 
				&sensors_data
			);
		}

		if (status == UAL_STATUS_OK) {
			UAL_SENSORS_DATA_STORAGE_Set(&sensors_data);
		}

		vTaskDelay(pdMS_TO_TICKS(CONFIG_SENSORS_DATA_SYNC_TIME_MS));
	}
}
