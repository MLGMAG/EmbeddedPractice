#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "sdkconfig.h"
#include "button_state.h"
#include "common.h"
#include "gpio_util.h"

QueueHandle_t BUTTON_IT_QUEUE;

static const char *TAG = "APP";

static void init() {
	gpio_config_t config;
	esp_err_t status;

	config.pin_bit_mask = (1 << CONFIG_LED_GPIO);
	config.mode = GPIO_MODE_OUTPUT;
	config.pull_up_en = GPIO_PULLUP_ENABLE;
	config.pull_down_en = GPIO_PULLDOWN_DISABLE;
	config.intr_type = GPIO_INTR_DISABLE;

	status = gpio_config(&config);
	if (status != ESP_OK) {
		ESP_LOGE(TAG, "Failed to config GPIO %d, status: %d", CONFIG_LED_GPIO, status);
		UAL_Error_Handler();
	}

	config.pin_bit_mask = (1 << CONFIG_INPUT_GPIO);
	config.mode = GPIO_MODE_INPUT;
	config.pull_up_en = GPIO_PULLUP_ENABLE;
	config.pull_down_en = GPIO_PULLDOWN_DISABLE;
	config.intr_type = GPIO_INTR_POSEDGE;
	status = gpio_config(&config);

	if (status != ESP_OK) {
		ESP_LOGE(TAG, "Failed to config GPIO %d, status: %d", CONFIG_INPUT_GPIO, status);
		UAL_Error_Handler();
	}
	
	gpio_install_isr_service(0);
	
	BUTTON_IT_QUEUE = xQueueCreate(4, sizeof(uint8_t));
}

void app_main(void) {
	init();
	
	GPIO_PinState led_state = GPIO_PIN_RESET;

	while (1) {
		UAL_BUTTON_STATE_t button_state = UAL_BUTTON_STATE_GetState();
		if (button_state == UAL_BUTTON_STATE_IDLE) {
			UAL_GPIO_UTIL_EnableButtonIt();
			uint8_t value;
			BaseType_t status = xQueueReceive(BUTTON_IT_QUEUE, &value, pdMS_TO_TICKS(10000));
			if (status == pdFAIL) {
				UAL_QueueTimeoutError_Handler();
				vTaskDelay(pdMS_TO_TICKS(100));
			} else if (status == pdPASS) {
				UAL_BUTTON_STATE_UpdateState(GPIO_PIN_SET);
				vTaskDelay(pdMS_TO_TICKS(5));
			}
		} else if (button_state == UAL_BUTTON_STATE_RELEASE) {
			ESP_LOGI(TAG, "Button is pressed!");
			led_state = led_state == GPIO_PIN_SET ? GPIO_PIN_RESET : GPIO_PIN_SET;
			UAL_GPIO_UTIL_SetLed(led_state);
			UAL_BUTTON_STATE_UpdateState(GPIO_PIN_RESET);
		} else {
			GPIO_PinState button_value = UAL_GPIO_UTIL_GetInputButtonState() == GPIO_PIN_RESET ? GPIO_PIN_SET : GPIO_PIN_RESET;
			UAL_BUTTON_STATE_UpdateState(button_value);
			vTaskDelay(pdMS_TO_TICKS(5));
		}
	}
}
