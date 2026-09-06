#include "task/command_handler_task.h"
#include "common.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include <string.h>
#include "hardware/gpio_util.h"
#include "service/sensors_data_storage.h"

#define GET_DATA_COMMAND "get data"
#define LED_ON_COMMAND "led on"
#define LED_OFF_COMMAND "led off"

#define JSON_DATA_FORMAT "{\"timestamp\":\"%d-%02d-%02d %02d:%02d:%02d\",\"data\":{\"temperature\":%.2f,\"humidity\":%d,\"pressure\":%d}}"

static const char *TAG = "COMMAND_HANDLER_TASK";

static char message_buffer[UART_TX_MSG_LEN_MAX];

extern QueueHandle_t COMMAND_QUEUE_HANDLER;
extern QueueHandle_t UART_TX_QUEUE_HANDLER;

static void send_uart_msg(const char *msg) {
	UART_TX_QUEUE_MSG_t uart_msg = {
		.msg = {0}
	};
	uint16_t len = strlen(msg);
	memcpy(uart_msg.msg, msg, len);
	uart_msg.msg[len] = '\n';
	uart_msg.msg[len + 1] = '\0';

	BaseType_t send_status =
		xQueueSend(UART_TX_QUEUE_HANDLER, &uart_msg, pdMS_TO_TICKS(50));
	if (send_status == errQUEUE_FULL) {
		ESP_LOGE(TAG, "Failed to send message to UART.");
	}
}

static void handle(const char *command) {
	if (strcmp(command, GET_DATA_COMMAND) == 0) {
		ESP_LOGI(TAG, "GET DATA command");
		
		SENSORS_DATA_t sensors_data = UAL_SENSORS_DATA_STORAGE_Get();
		sprintf((char*) message_buffer, JSON_DATA_FORMAT,
				sensors_data.date.year, sensors_data.date.month,
				sensors_data.date.day, sensors_data.time.hour,
				sensors_data.time.minute, sensors_data.time.second,
				sensors_data.weather_data.temp,
				sensors_data.weather_data.humidity,
				sensors_data.weather_data.pressure);

		send_uart_msg(message_buffer);
	} else if (strcmp(command, LED_ON_COMMAND) == 0) {
		ESP_LOGI(TAG, "LED ON command");
		
		UAL_GPIO_UTIL_SetLed(GPIO_PIN_SET);
		
		sprintf(message_buffer, "Led is turned on");
		send_uart_msg(message_buffer);
	} else if (strcmp(command, LED_OFF_COMMAND) == 0) {
		ESP_LOGI(TAG, "LED OFF command");
		
		UAL_GPIO_UTIL_SetLed(GPIO_PIN_RESET);
		
		sprintf(message_buffer, "Led is turned off");
		send_uart_msg(message_buffer);
	} else {
		ESP_LOGW(TAG, "Command is unsupported: %s", command);
		sprintf(message_buffer, "Command is unsupported: %s", command);
		send_uart_msg(message_buffer);
	}
}

void UAL_COMMAND_HANDLER_TASK_Start(void *pvParameters) {
	COMMAND_QUEUE_MSG_t queue_msg;
	TickType_t get_msg_delay = pdMS_TO_TICKS(10000);

	while (1) {
		if (xQueueReceive(COMMAND_QUEUE_HANDLER, &queue_msg, get_msg_delay)) {
			handle((char *)queue_msg.command);
		}

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
