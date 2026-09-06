#include "hardware/gpio_util.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "sdkconfig.h"

extern QueueHandle_t BUTTON_IT_QUEUE;

static const char *TAG = "GPIO_UTILS";

static GPIO_PinState led_state = GPIO_PIN_RESET;

static void IRAM_ATTR button_isr(void *arg) {
	uint8_t value = 1;
	BaseType_t higher_priority_task_woken = pdFALSE;

	xQueueSendFromISR(BUTTON_IT_QUEUE, (void *)&value,
					  &higher_priority_task_woken);

	UAL_GPIO_UTIL_DisableButtonIt();
}

void UAL_GPIO_UTIL_DisableButtonIt() {
	esp_err_t status = gpio_isr_handler_remove(CONFIG_INPUT_GPIO);
	if (status != ESP_OK) {
		ESP_LOGE(TAG, "Disable button interrupt is failed, status: %d", status);
		UAL_Error_Handler();
	}
}

void UAL_GPIO_UTIL_EnableButtonIt() {
	esp_err_t status =
		gpio_isr_handler_add(CONFIG_INPUT_GPIO, button_isr, NULL);
	if (status != ESP_OK) {
		ESP_LOGE(TAG, "Enable button interrupt is failed, status: %d", status);
		UAL_Error_Handler();
	}
}

GPIO_PinState UAL_GPIO_UTIL_GetInputButtonState() {
	return gpio_get_level(CONFIG_INPUT_GPIO) == 1 ? GPIO_PIN_SET
												  : GPIO_PIN_RESET;
}

GPIO_PinState UAL_GPIO_UTIL_GetLed() {
	return led_state;
}

void UAL_GPIO_UTIL_SetLed(GPIO_PinState value) {
	led_state = value;
	gpio_set_level(CONFIG_LED_GPIO, value == GPIO_PIN_SET ? 1 : 0);
}
