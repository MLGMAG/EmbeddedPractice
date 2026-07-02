#ifndef __COMMONS_H
#define __COMMONS_H

#include "stdint.h"

#define UAL_TRACK_PATH_MAX_LEN 128
#define UAL_TRACK_FILENAME_MAX_LEN 64

#define UAL_DISPLAY_PLAYLIST_TITLE  "      PLAYLIST      "
#define UAL_DISPLAY_NO_TRACKS "       (empty)      "

typedef enum {
	UAL_STATUS_OK, UAL_STATUS_ERROR,
} UAL_STATUS_t;

typedef enum {
	UAL_MANAGER_MSG_TYPE_BUTTON_PRESS, UAL_MANAGER_MSG_TYPE_TRACK_FINISH
} UAL_MANAGER_MSG_TYPE_t;

typedef struct {
	uint64_t data_section_pointer;
	uint32_t file_size;
	uint32_t data_size;
	uint32_t sample_rate;
	uint32_t bytes_per_second;
	uint16_t audio_format;
	uint16_t channels_num;
	uint16_t bytes_per_sample;
	uint16_t bits_per_sample_per_channel;
} WAV_FILE_METADATA_t;

typedef struct {
	char path[UAL_TRACK_PATH_MAX_LEN];
	char filename[UAL_TRACK_FILENAME_MAX_LEN];
	WAV_FILE_METADATA_t metadata;
} UAL_TRACK_t;

typedef struct {
	char path[UAL_TRACK_PATH_MAX_LEN];
	uint64_t data_section_pointer;
	uint32_t file_size;
} UAL_TRACK_MSG_t;

typedef struct {
	UAL_MANAGER_MSG_TYPE_t type;
} UAL_MANAGER_MSG_t;

void UAL_Error_Handler(void);

#endif /* __COMMONS_H */
