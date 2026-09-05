#include "uart_rx_task.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "hardware/uart_util.h"
#include <stdint.h>

#define MAX_COMMAND_LEN 32

typedef enum {
	COMMAND_AVAILABLE,
	COMMAND_UNAVAILABLE,
} PARSE_STATUS_t;

static const char *TAG = "UART_RX_TASK";

static uint8_t raw_data_buffer[MAX_COMMAND_LEN] = {0};
static uint8_t data_buffer[MAX_COMMAND_LEN] = {0};
static uint8_t data_pointer = 0;

static PARSE_STATUS_t parse_data(uint8_t *in_buffer, uint32_t len) {
	PARSE_STATUS_t status = COMMAND_UNAVAILABLE;
	
	if (len >= MAX_COMMAND_LEN - 1) {
		ESP_LOGE(TAG, "Command is too big: %d", len);
	} else if (in_buffer[len - 1] == '\n') {
		if (data_pointer == 0) {
			ESP_LOGI(TAG, "Command is empty!");
		} else {
			data_buffer[data_pointer] = '\0';
			data_pointer = 0;
			status = COMMAND_AVAILABLE;
		}
	} else {
		if (data_pointer + len >= MAX_COMMAND_LEN - 1) {
			ESP_LOGI(TAG, "Buffer overflow!");
			data_pointer = 0;
		}
		for (int i = 0; i < len; i++) {
			data_buffer[data_pointer + i] = in_buffer[i];
		}
		data_pointer += len;
	}
	
	return status;
}

void UAL_UART_RX_TASK_Start(void *pvParameters) {
	while (1) {
		uint16_t data_len = UAL_UART_UTIL_Receive(
			raw_data_buffer, 
			MAX_COMMAND_LEN, 
			pdMS_TO_TICKS(10000)
		);

		if (data_len > 0) {
			PARSE_STATUS_t parse_status = parse_data(raw_data_buffer, data_len);
			if (parse_status == COMMAND_AVAILABLE) {
				ESP_LOGI(TAG, "Received command: %s", data_buffer);
			}
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
