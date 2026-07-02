#include "wav_utils.h"
#include "fatfs.h"
#include "string.h"

UAL_WAV_UTILS_EXTRACT_DETAILS_t UAL_WAV_FILE_ExtractMetadata(const char *fname) {
	FIL file;

	UAL_WAV_UTILS_EXTRACT_DETAILS_t result;
	result.status = UAL_STATUS_ERROR;

	FRESULT res = f_open(&file, fname, FA_READ);
	if (res != FR_OK) {
		return result;
	}

	unsigned int bytesRead;

	uint8_t header[36];
	res = f_read(&file, header, sizeof(header), &bytesRead);
	if (res != FR_OK) {
		f_close(&file);
		return result;
	}

	if (memcmp((const char*) header, "RIFF", 4) != 0) {
		f_close(&file);
		return result;
	}

	if (memcmp((const char*) header + 8, "WAVE", 4) != 0) {
		f_close(&file);
		return result;
	}

	if (memcmp((const char*) header + 12, "fmt ", 4) != 0) {
		f_close(&file);
		return result;
	}

	uint32_t block_size = header[16] | (header[17] << 8) | (header[18] << 16)
			| (header[19] << 24);
	if (block_size != 16) {
		f_close(&file);
		return result;
	}

	uint32_t file_size = 8
			+ (header[4] | (header[5] << 8) | (header[6] << 16)
					| (header[7] << 24));
	uint16_t audio_format = header[20] | (header[21] << 8);
	uint16_t channels_num = header[22] | (header[23] << 8);
	uint32_t sample_rate = header[24] | (header[25] << 8) | (header[26] << 16)
			| (header[27] << 24);
	uint32_t bytes_per_second = header[28] | (header[29] << 8)
			| (header[30] << 16) | (header[31] << 24);
	uint16_t bytes_per_sample = header[32] | (header[33] << 8);
	uint16_t bits_per_sample_per_channel = header[34] | (header[35] << 8);

	uint8_t is_sub_header = 1;
	uint8_t sub_header_metadata[8];
	while (is_sub_header == 1) {
		res = f_read(&file, sub_header_metadata, sizeof(sub_header_metadata),
				&bytesRead);
		if (res != FR_OK) {
			f_close(&file);
			return result;
		}
		if (memcmp((const char*) sub_header_metadata, "data", 4) == 0) {
			is_sub_header = 0;
		}
		if (is_sub_header) {
			uint32_t next_header_body_size = sub_header_metadata[4]
					| (sub_header_metadata[5] << 8)
					| (sub_header_metadata[5] << 16)
					| (sub_header_metadata[6] << 24);

			res = f_lseek(&file, f_tell(&file) + next_header_body_size);
			if (res != FR_OK) {
				f_close(&file);
				return result;
			}
		}
	}

	uint32_t data_size = sub_header_metadata[4] | (sub_header_metadata[5] << 8)
			| (sub_header_metadata[6] << 16) | (sub_header_metadata[7] << 24);

	uint64_t data_section_pointer = f_tell(&file);

	f_close(&file);

	result.metadata.file_size = file_size;
	result.metadata.audio_format = audio_format;
	result.metadata.channels_num = channels_num;
	result.metadata.sample_rate = sample_rate;
	result.metadata.bytes_per_second = bytes_per_second;
	result.metadata.bytes_per_sample = bytes_per_sample;
	result.metadata.bits_per_sample_per_channel = bits_per_sample_per_channel;
	result.metadata.data_size = data_size;
	result.metadata.data_section_pointer = data_section_pointer;

	result.status = UAL_STATUS_OK;

	return result;
}
