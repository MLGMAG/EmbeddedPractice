#include <fatfs_logger.h>

#include <string.h>

static UAL_FATFS_LOGGER_InitStruct_t init_params;

static UAL_FATFS_LOGGER_Status_t UAL_FATFS_MountDrive(FATFS *fatfs);
static UAL_FATFS_LOGGER_Status_t UAL_FATFS_InitLogDir(const char* log_dir_title);

UAL_FATFS_LOGGER_Status_t UAL_FATFS_LOGGER_Init(UAL_FATFS_LOGGER_InitStruct_t *input) {
	memcpy(&init_params, input, sizeof(UAL_FATFS_LOGGER_InitStruct_t));

	UAL_FATFS_LOGGER_Status_t status;
	status = UAL_FATFS_MountDrive(&(init_params.fatfs));

	if(status != UAL_FATFS_LOGGER_STATUS_OK){
		return status;
	}

	return UAL_FATFS_InitLogDir(init_params.log_dir_title);
}

UAL_FATFS_LOGGER_Status_t UAL_FATFS_LOGGER_Write(char *message) {
	FRESULT fres;
	FIL fil;
	fres = f_open(&fil, init_params.log_file_path, FA_WRITE | FA_OPEN_APPEND);
	if (fres != FR_OK) {
		return UAL_FATFS_LOGGER_STATUS_ERROR;
	}

	UINT bytesWrote;
	uint16_t len = strlen((char*) message);
	fres = f_write(&fil, message, len, &bytesWrote);
	if (fres != FR_OK) {
		return UAL_FATFS_LOGGER_STATUS_ERROR;
	}

	f_close(&fil);

	return UAL_FATFS_LOGGER_STATUS_OK;
}

static UAL_FATFS_LOGGER_Status_t UAL_FATFS_MountDrive(FATFS *fatfs) {
	return f_mount(fatfs, "", 1) == FR_OK ? UAL_FATFS_LOGGER_STATUS_OK : UAL_FATFS_LOGGER_STATUS_ERROR;
}

static UAL_FATFS_LOGGER_Status_t UAL_FATFS_InitLogDir(const char* log_dir_title) {
	FRESULT fres;
	FILINFO fno;

	fres = f_stat(log_dir_title, &fno);
	if (fres == FR_NO_FILE) {
		fres = f_mkdir(log_dir_title);
	}

	return fres == FR_OK ? UAL_FATFS_LOGGER_STATUS_OK : UAL_FATFS_LOGGER_STATUS_ERROR;
}
