#include "fatfs_utility.h"
#include <string.h>

UAL_FATFS_Status_t UAL_FATFS_UTILITY_MountDrive(FATFS *fatfs) {
	return f_mount(fatfs, "", 1) == FR_OK ?
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

UAL_FATFS_Status_t UAL_FATFS_UTILITY_Write(const char *filepath,
		const char *message) {
	FRESULT fres;
	FIL fil;
	fres = f_open(&fil, filepath, FA_WRITE | FA_OPEN_APPEND);
	if (fres != FR_OK) {
		return UAL_FATFS_STATUS_ERROR;
	}

	UINT bytesWrote;
	uint16_t len = strlen((char*) message);
	fres = f_write(&fil, message, len, &bytesWrote);
	if (fres != FR_OK) {
		return UAL_FATFS_STATUS_ERROR;
	}

	f_close(&fil);

	return UAL_FATFS_STATUS_OK;
}
