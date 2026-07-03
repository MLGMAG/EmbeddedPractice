#ifndef __FATFS_UTILITY_H
#define __FATFS_UTILITY_H

#include "fatfs.h"

typedef enum {
	UAL_FATFS_STATUS_OK, UAL_FATFS_STATUS_ERROR
} UAL_FATFS_Status_t;

UAL_FATFS_Status_t UAL_FATFS_UTILITY_MountDrive(FATFS *fatfs);

UAL_FATFS_Status_t UAL_FATFS_UTILITY_CreateDir(const char *dir_title);

UAL_FATFS_Status_t UAL_FATFS_UTILITY_Write(const char *filepath,
		const char *message);

#endif /* __FATFS_UTILITY_H */
