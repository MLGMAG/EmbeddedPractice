#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "sdkconfig.h"

static const char *TAG = "APP";

void error_handler() {
	while (1) {
	}
}

void init() {
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
		error_handler();
	}

	config.pin_bit_mask = (1 << CONFIG_INPUT_GPIO);
	config.mode = GPIO_MODE_INPUT;
	config.pull_up_en = GPIO_PULLUP_ENABLE;
	config.pull_down_en = GPIO_PULLDOWN_DISABLE;
	config.intr_type = GPIO_INTR_DISABLE;
	status = gpio_config(&config);

	if (status != ESP_OK) {
		ESP_LOGE(TAG, "Failed to config GPIO %d, status: %d", CONFIG_INPUT_GPIO, status);
		error_handler();
	}
}

void app_main(void) {
	init();

	while (1) {
		int input = gpio_get_level(CONFIG_INPUT_GPIO);

		if (input == 0) {
			gpio_set_level(CONFIG_LED_GPIO, 1);
		} else {
			gpio_set_level(CONFIG_LED_GPIO, 0);
		}

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
