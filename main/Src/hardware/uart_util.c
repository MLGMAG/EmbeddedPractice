#include "hardware/uart_util.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "sdkconfig.h"

static const char *TAG = "UART_UTIL";

QueueHandle_t UAL_UART_UTIL_QUEUE;

void UAL_UART_UTIL_Init() {
	esp_err_t status;

	status = uart_driver_install(
		UAL_UART_UTIL_UART_PORT,
		CONFIG_UART_RX_BUFFER_SIZE, 
		CONFIG_UART_TX_BUFFER_SIZE, 
		10, 
		&UAL_UART_UTIL_QUEUE, 
		0
	);
	ESP_ERROR_CHECK(status);

	uart_config_t uart_config = {
	    .baud_rate = CONFIG_UART_BAUD_RATE,
	    .data_bits = UART_DATA_8_BITS,
	    .parity = UART_PARITY_DISABLE,
	    .stop_bits = UART_STOP_BITS_1,
	    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	};
	status = uart_param_config(UAL_UART_UTIL_UART_PORT, &uart_config);
	ESP_ERROR_CHECK(status);

	status = uart_set_pin(
		UAL_UART_UTIL_UART_PORT,
		CONFIG_UART_TX_PORT,
		CONFIG_UART_RX_PORT, 
		UART_PIN_NO_CHANGE, 
		UART_PIN_NO_CHANGE, 
		UART_PIN_NO_CHANGE, 
		UART_PIN_NO_CHANGE
	);
	ESP_ERROR_CHECK(status);
}

UAL_STATUS_t UAL_UART_UTIL_Transmit(const uint8_t *data, uint16_t len) {
	int result = uart_write_bytes(
		UAL_UART_UTIL_UART_PORT, 
		(const char*)data, 
		len
	);
	
	if (result == -1) {
		return UAL_STATUS_ERROR;
	}
	
	return UAL_STATUS_OK;
}

uint16_t UAL_UART_UTIL_Receive(const uint8_t *buffer, uint16_t buffer_size, uint32_t delay) {
	uart_event_t event;
	event.size = 0;

	if (xQueueReceive(UAL_UART_UTIL_QUEUE, (void *)&event, delay)) {
		if (buffer_size > event.size) {
			switch (event.type) {
			case UART_DATA:
				int status =
					uart_read_bytes(UAL_UART_UTIL_UART_PORT, (void *) buffer,
									event.size, portMAX_DELAY);
				if (status == -1) {
					ESP_LOGE(TAG, "Error occur during read.", event.size);
				}
				break;
			default:
				ESP_LOGW(TAG, "Skip UART event. Event type: %d.", event.type);
				break;
			}
		} else {
			ESP_LOGE(TAG, "Received size %d is greater than buffer size %d", event.size, buffer_size);
			event.size = 0;
		}
	}
	
	return event.size;
}
