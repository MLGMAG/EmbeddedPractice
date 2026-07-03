#ifndef __FATFS_LOGGER_H
#define __FATFS_LOGGER_H

#include <stdint.h>
#include "fatfs.h"

typedef struct {
	FATFS fatfs;
	char log_dir_title[32];
	char log_file_path[64];
} UAL_FATFS_LOGGER_InitStruct_t;

typedef enum
{
  UAL_FATFS_LOGGER_STATUS_OK,
  UAL_FATFS_LOGGER_STATUS_ERROR
} UAL_FATFS_LOGGER_Status_t;

UAL_FATFS_LOGGER_Status_t UAL_FATFS_LOGGER_Init(UAL_FATFS_LOGGER_InitStruct_t *input);

UAL_FATFS_LOGGER_Status_t UAL_FATFS_LOGGER_Write(char *message);

#endif /* __FATFS_LOGGER_H */
