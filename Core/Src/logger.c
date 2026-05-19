#include "logger.h"

const char* LOGGER_trace_prefix = "[TRACE]: ";
const char* LOGGER_debug_prefix = "[DEBUG]: ";
const char* LOGGER_info_prefix = "[INFO]: ";
const char* LOGGER_warn_prefix = "[WARN]: ";
const char* LOGGER_error_prefix = "[ERROR]: ";

#if defined(LOGGER_USE_SWD)
static LOGGER_Error_t LOGGER_write(const char* message, const char* prefix) {
	printf("%s%s\n", prefix, message);
}
#elif defined(LOGGER_USE_UART)
static uint8_t LOGGER_buffer[LOGGER_UART_BUFFER_LEN];

static LOGGER_Error_t LOGGER_write(const char* message, const char* prefix) {
	uint16_t prefix_len = strlen((char *) prefix);
	uint16_t message_len = strlen((char *) message);

	if (prefix_len + message_len + 1 > LOGGER_UART_BUFFER_LEN) {
		return LOGGER_ERR;
	}

	sprintf((char *) LOGGER_buffer, "%s%s\n", prefix, message);
	uint16_t log_message_len = strlen((char *) LOGGER_buffer);

	HAL_StatusTypeDef status = HAL_UART_Transmit(&LOGGER_UART_PORT, LOGGER_buffer, log_message_len, HAL_MAX_DELAY);
	if (status != HAL_OK) {
		return LOGGER_ERR;
	}

	return LOGGER_OK;
}
#elif defined(LOGGER_USE_SPI_SD_CARD)
static void LOGGER_write(const char* message, const char* prefix) {

}
#else
#error "You should define LOGGER_USE_SWD, LOGGER_USE_UART or LOGGER_USE_SPI_SD_CARD macro"
#endif

void LOGGER_trace(const char* message) {
	if (LOGGER_Level <= LOGGER_TRACE) {
		LOGGER_write(message, LOGGER_trace_prefix);
	}
}

void LOGGER_debug(const char* message) {
	if (LOGGER_Level <= LOGGER_DEBUG) {
		LOGGER_write(message, LOGGER_debug_prefix);
	}
}

void LOGGER_info(const char* message) {
	if (LOGGER_Level <= LOGGER_INFO) {
		LOGGER_write(message, LOGGER_info_prefix);
	}
}

void LOGGER_warn(const char* message) {
	if (LOGGER_Level <= LOGGER_WARN) {
		LOGGER_write(message, LOGGER_warn_prefix);
	}
}

void LOGGER_error(const char* message) {
	if (LOGGER_Level <= LOGGER_ERROR) {
		LOGGER_write(message, LOGGER_error_prefix);
	}
}
