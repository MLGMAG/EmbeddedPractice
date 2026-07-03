#include <fatfs_logger.h>

#include <string.h>

static UAL_FATFS_LOGGER_InitStruct_t init_params;

UAL_FATFS_Status_t UAL_FATFS_LOGGER_Init(UAL_FATFS_LOGGER_InitStruct_t *input) {
	if (strstr(input->log_dir_title, "/") != NULL) {
		return UAL_FATFS_STATUS_ERROR;
	}

	if (strstr(input->log_file_path, input->log_dir_title) == NULL) {
		return UAL_FATFS_STATUS_ERROR;
	}

	memcpy(&init_params, input, sizeof(UAL_FATFS_LOGGER_InitStruct_t));

	UAL_FATFS_Status_t status;
	status = UAL_FATFS_UTILITY_MountDrive(&(init_params.fatfs));

	if (status != UAL_FATFS_STATUS_OK) {
		return status;
	}

	return UAL_FATFS_UTILITY_CreateDir(init_params.log_dir_title);
}

UAL_FATFS_Status_t UAL_FATFS_LOGGER_Write(char *message) {
	return UAL_FATFS_UTILITY_Write(init_params.log_file_path, message);
}
