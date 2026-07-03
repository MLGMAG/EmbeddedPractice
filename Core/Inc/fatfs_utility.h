#ifndef __FATFS_UTILITY_H
#define __FATFS_UTILITY_H

#include "fatfs.h"

typedef enum {
	UAL_FATFS_STATUS_OK, UAL_FATFS_STATUS_ERROR
} UAL_FATFS_Status_t;

typedef struct {
	uint32_t bytes_read;
	uint32_t file_len;
	UAL_FATFS_Status_t status;
} UAL_FATFS_ReadResult_t;

typedef struct {
	uint32_t bytes_wrote;
	uint32_t file_len;
	UAL_FATFS_Status_t status;
} UAL_FATFS_WriteResult_t;

UAL_FATFS_Status_t UAL_FATFS_UTILITY_MountDrive(FATFS *fatfs);

UAL_FATFS_Status_t UAL_FATFS_UTILITY_CreateDir(const char *dir_title);

uint32_t UAL_FATFS_UTILITY_GetFileSize(const char *filepath);

UAL_FATFS_WriteResult_t UAL_FATFS_UTILITY_Write(const char *filepath,
		const uint8_t *buffer, const uint16_t len);

UAL_FATFS_ReadResult_t UAL_FATFS_UTILITY_Read(const char *filepath,
		uint8_t *buffer, const uint32_t offset, uint32_t read_len);

#endif /* __FATFS_UTILITY_H */
