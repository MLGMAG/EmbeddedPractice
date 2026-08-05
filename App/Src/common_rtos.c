#include "common_rtos.h"
#include "common.h"

void UAL_MessageQueue_CheckStatus(const osStatus_t *status) {
	if (*status == osErrorTimeout) {
		UAL_QueueTimeoutError_Handler();
	}

	if (*status != osOK && *status != osErrorTimeout) {
		UAL_Error_Handler();
	}
}
