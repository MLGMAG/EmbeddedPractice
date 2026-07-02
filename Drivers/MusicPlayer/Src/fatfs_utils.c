#include "fatfs_utils.h"
#include <string.h>

UAL_FATFS_Status_t UAL_FATFS_UTILITY_MountDrive(FATFS *fatfs) {
	return f_mount(fatfs, "", 1) == FR_OK ?
			UAL_FATFS_STATUS_OK : UAL_FATFS_STATUS_ERROR;
}

UAL_FATFS_Status_t UAL_FATFS_UTILITY_UnmountDrive() {
	return f_mount(NULL, "", 0) == FR_OK ?
			UAL_FATFS_STATUS_OK : UAL_FATFS_STATUS_ERROR;
}

UAL_FATFS_Status_t UAL_FATFS_UTILITY_CreateDir(const char *dir_title) {
	FRESULT fres;
	FILINFO fno;

	fres = f_stat(dir_title, &fno);
	if (fres == FR_NO_FILE) {
		fres = f_mkdir(dir_title);
	}

	return fres == FR_OK ? UAL_FATFS_STATUS_OK : UAL_FATFS_STATUS_ERROR;
}

uint32_t UAL_FATFS_UTILITY_GetFileSize(const char *filepath) {
	FRESULT fres;
	FIL fil;

	fres = f_open(&fil, filepath, FA_READ);
	if (fres != FR_OK) {
		return -1;
	}

	f_close(&fil);

	return f_size(&fil);
}

UAL_FATFS_WriteResult_t UAL_FATFS_UTILITY_Write(const char *filepath,
		const uint8_t *buffer, const uint16_t len) {
	FRESULT fres;
	FIL fil;

	UAL_FATFS_WriteResult_t result;
	result.file_len = 0;
	result.bytes_wrote = 0;
	result.status = UAL_FATFS_STATUS_ERROR;

	fres = f_open(&fil, filepath, FA_WRITE | FA_OPEN_APPEND);
	if (fres != FR_OK) {
		return result;
	}

	uint32_t bytes_wrote;
	fres = f_write(&fil, buffer, len, (UINT*) &bytes_wrote);
	if (fres != FR_OK) {
		return result;
	}

	result.file_len = f_size(&fil);
	result.bytes_wrote = bytes_wrote;
	result.status = UAL_FATFS_STATUS_OK;

	f_close(&fil);

	return result;
}

UAL_FATFS_ReadResult_t UAL_FATFS_UTILITY_Read(const char *filepath, uint8_t *buffer,
		const uint32_t offset, uint32_t read_len) {
	FRESULT fres;
	FIL fil;

	UAL_FATFS_ReadResult_t result;
	result.file_len = 0;
	result.bytes_read = 0;
	result.status = UAL_FATFS_STATUS_ERROR;

	fres = f_open(&fil, filepath, FA_READ);
	if (fres != FR_OK) {
		return result;
	}

	fres = f_lseek(&fil, offset);
	if (fres != FR_OK) {
		return result;
	}

	uint32_t bytes_read;

	fres = f_read(&fil, buffer, read_len, (UINT*) &bytes_read);
	if (fres != FR_OK) {
		return result;
	}

	result.file_len = f_size(&fil);
	result.bytes_read = bytes_read;
	result.status = UAL_FATFS_STATUS_OK;

	f_close(&fil);

	return result;
}
