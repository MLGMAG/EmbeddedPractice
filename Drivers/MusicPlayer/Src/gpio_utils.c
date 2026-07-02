#include "gpio_utils.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

extern osSemaphoreId_t BUTTON_SEMAPHORE_ID;
extern uint8_t disable_button_isr;

static GPIO_TypeDef *GPIO_PORT = GPIOB;
static uint16_t GPIO_PIN = GPIO_PIN_12;

uint8_t UAL_GPIO_UTILS_GetInputButtonState() {
	return HAL_GPIO_ReadPin(GPIO_PORT, GPIO_PIN) == GPIO_PIN_SET ? 0 : 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_12 && disable_button_isr == 0) {
		osSemaphoreRelease(BUTTON_SEMAPHORE_ID);
		disable_button_isr = 1;
	}
}
