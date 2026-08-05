#include "task/display_task.h"
#include "service/display_service.h"
#include "common.h"
#include "cmsis_os.h"

extern osMessageQueueId_t display_queueHandle;

static uint8_t is_screen_enabled = 0;

static void init() {
	UAL_DISPLAY_SERVICE_Init();
	UAL_DISPLAY_SERVICE_Screen_Disable();
	UAL_DISPLAY_SERVICE_SendorData_FillTitles();
}

void UAL_DISPLAY_TASK_Start(void *argument) {
	SENSORS_DATA_t sensors_data;
	osStatus_t status;
	uint32_t getMessage_delay = pdMS_TO_TICKS(10000);

	init();

	while (1) {
		status = osMessageQueueGet(display_queueHandle, &sensors_data, NULL,
				getMessage_delay);

		if (status == osOK) {
			UAL_DISPLAY_SERVICE_SensorData_Update(&sensors_data);
			if (!is_screen_enabled) {
				UAL_DISPLAY_SERVICE_Screen_Enable();
			}
		} else if (status != osErrorTimeout) {
			UAL_Error_Handler();
		}
		osDelay(1);
	}
}
