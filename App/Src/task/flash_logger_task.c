#include "task/flash_logger_task.h"
#include "cmsis_os.h"
#include "service/sensors_data_storage.h"

#define FLASH_WRITE_PER_HOUR 60

void UAL_FLASH_LOGGER_TASK_Start(void *argument) {
	uint32_t task_delay = pdMS_TO_TICKS(3600000 / FLASH_WRITE_PER_HOUR);

	/* Infinite loop */
	while (1) {
		UAL_SENSORS_DATA_STORAGE_PeristToFlash();
		osDelay(task_delay);
	}
}
