#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED1_GPIO 2
#define LED2_GPIO 4
#define LED3_GPIO 13

#define LED_TASKS_COUNT 3
#define LED1_TASK_TAG "LED_TASK1"
#define LED2_TASK_TAG "LED_TASK2"
#define LED3_TASK_TAG "LED_TASK3"

static const char *APP_TAG = "MAIN";

typedef struct {
	int16_t toggle_timeout;
	const char *tag;
	int8_t gpio_pin;
	int16_t delay;
} task_led_params;

static const task_led_params task1_params = {.toggle_timeout = 200,
											 .tag = LED1_TASK_TAG,
											 .gpio_pin = LED1_GPIO,
											 .delay = 100};

static const task_led_params task2_params = {.toggle_timeout = 500,
											 .tag = LED2_TASK_TAG,
											 .gpio_pin = LED2_GPIO,
											 .delay = 100};

static const task_led_params task3_params = {.toggle_timeout = 1000,
											 .tag = LED3_TASK_TAG,
											 .gpio_pin = LED3_GPIO,
											 .delay = 100};

static const task_led_params tasks_led_params[LED_TASKS_COUNT] = {
	task1_params, task2_params, task3_params};

void init(void) {
	gpio_reset_pin(LED1_GPIO);
	gpio_set_direction(LED1_GPIO, GPIO_MODE_OUTPUT);

	gpio_reset_pin(LED2_GPIO);
	gpio_set_direction(LED2_GPIO, GPIO_MODE_OUTPUT);

	gpio_reset_pin(LED3_GPIO);
	gpio_set_direction(LED3_GPIO, GPIO_MODE_OUTPUT);
}

static int64_t timer_get_time_ms() { return esp_timer_get_time() / 1000; }

void task_led(void *pvParameters) {
	task_led_params *params = (task_led_params *)pvParameters;

	int64_t last_toggle_ms = timer_get_time_ms();
	int8_t last_value = 1;
	while (1) {
		int64_t current_time = timer_get_time_ms();
		int64_t current_timeout = current_time - last_toggle_ms;
		if (current_timeout >= params->toggle_timeout) {
			last_value = last_value == 1 ? 0 : 1;
			last_toggle_ms = current_time;

			ESP_LOGI(params->tag, "LED is toggled");
			gpio_set_level(params->gpio_pin, last_value);
		}
		vTaskDelay(pdMS_TO_TICKS(params->delay));
	}
}

void app_main(void) {
	init();

	for (int i = 0; i < LED_TASKS_COUNT; i++) {
		BaseType_t xReturned =
			xTaskCreatePinnedToCore(task_led, tasks_led_params[i].tag, 2048,
									(void *)&tasks_led_params[i], 1, NULL, 1);

		if (xReturned != pdPASS) {
			ESP_LOGE(APP_TAG, "Task not created!");
		}
	}
}
