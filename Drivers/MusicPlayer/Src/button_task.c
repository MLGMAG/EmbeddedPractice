#include "button_task.h"
#include "button_state.h"
#include "gpio_utils.h"
#include "cmsis_os.h"
#include "commons.h"

#define DOUBLE_CLICK_DELAY 100

extern osSemaphoreId_t BUTTON_SEMAPHORE_ID;
extern osMessageQueueId_t MANAGER_QUEUE;

uint8_t disable_button_isr = 0;

static void update_button_state() {
	uint8_t button_value = UAL_GPIO_UTILS_GetInputButtonState();
	UAL_BUTTON_STATE_UpdateState(button_value);
	osDelay(5);
}

void UAL_BUTTON_TASK_Start(void *argument) {
	osStatus_t status;
	uint32_t timeout = pdMS_TO_TICKS(10000);

	while (1) {
		UAL_BUTTON_STATE_t button_state = UAL_BUTTON_STATE_GetState();

		if (button_state == UAL_BUTTON_STATE_IDLE) {
			disable_button_isr = 0;

			status = osSemaphoreAcquire(BUTTON_SEMAPHORE_ID, timeout);
			if (status != osOK && status != osErrorTimeout) {
				UAL_Error_Handler();
			}
		}

		if (button_state == UAL_BUTTON_STATE_RELEASE) {
			UAL_MANAGER_MSG_t manager_msg;
			manager_msg.type = UAL_MANAGER_MSG_TYPE_BUTTON_PRESS;

			status = osMessageQueuePut(MANAGER_QUEUE, &manager_msg, 0U,
					pdMS_TO_TICKS(100));
			if (status != osOK) {
				UAL_Error_Handler();
			}
		}

		update_button_state();
		osDelay(1);
	}
}
