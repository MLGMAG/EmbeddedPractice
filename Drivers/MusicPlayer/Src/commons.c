#include "commons.h"
#include "cmsis_os.h"

void UAL_Error_Handler(void) {
	while (1) {
		osDelay(pdMS_TO_TICKS(10000));
	}
}
