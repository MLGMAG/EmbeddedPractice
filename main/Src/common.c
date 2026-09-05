#include "common.h"
#include "esp_log.h"

static const char *TAG = "COMMON";

void UAL_QueueTimeoutError_Handler(void) {
}

void UAL_Error_Handler(void) {
	ESP_LOGE(TAG, "Critical occurred. Stop the execution!");
	while (1) {
	}
}
