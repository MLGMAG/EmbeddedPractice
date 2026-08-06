#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"

static const char *TAG = "APP";

static int toggle_value(const int value) { return value == 0 ? 1 : 0; }

void app_main(void) {
	gpio_config_t config = {
		.pin_bit_mask = 1ULL << CONFIG_BLINK_GPIO,
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
	};

	esp_err_t esp_err = gpio_config(&config);
	if (esp_err != ESP_OK) {
		ESP_LOGE(TAG, "GPIO initialization error code: %d", esp_err);
	}

	int gpio_output_level = 0;
	while (1) {
		ESP_LOGI(TAG, "Switch led from %d to %d.", gpio_output_level,
				 toggle_value(gpio_output_level));
		gpio_output_level = toggle_value(gpio_output_level);
		gpio_set_level(CONFIG_BLINK_GPIO, gpio_output_level);
		vTaskDelay(pdMS_TO_TICKS(CONFIG_BLINK_PERIOD));
	}
}
