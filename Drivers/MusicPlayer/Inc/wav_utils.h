#ifndef __WAV_UTILS_H
#define __WAV_UTILS_H

#include "commons.h"

typedef struct {
	WAV_FILE_METADATA_t metadata;
	UAL_STATUS_t status;
} UAL_WAV_UTILS_EXTRACT_DETAILS_t;

UAL_WAV_UTILS_EXTRACT_DETAILS_t UAL_WAV_FILE_ExtractMetadata(const char *fname);

#endif /* __WAV_UTILS_H */
