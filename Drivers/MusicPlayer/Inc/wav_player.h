#ifndef __WAV_PLAYER_H
#define __WAV_PLAYER_H

#include "commons.h"

#define UAL_WAV_PLAYER_BUFFER_LEN 2048

void UAL_WAV_PLAYER_Init();

UAL_STATUS_t UAL_WAV_PLAYER_ValidateMetadata(const WAV_FILE_METADATA_t *metadata);

UAL_STATUS_t UAL_WAV_PLAYER_Play(const char *fname,
		const uint64_t data_section_pointer, const uint32_t data_size);

uint32_t UAL_WAV_PLAYER_GetRemainData();

void UAL_WAV_PLAYER_Stop();

#endif /* __WAV_PLAYER_H */
