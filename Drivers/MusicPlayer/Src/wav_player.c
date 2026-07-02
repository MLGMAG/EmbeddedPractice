#include "wav_player.h"
#include "fatfs.h"
#include "i2s_utils.h"

osSemaphoreId_t PLAYER_SEMAPHORE_ID;
volatile uint8_t continue_i2s_transition = 0;
volatile uint16_t *signal_play_buff = NULL;
volatile uint16_t *signal_read_buff = NULL;
volatile uint16_t signal_buff1[UAL_WAV_PLAYER_BUFFER_LEN];
volatile uint16_t signal_buff2[UAL_WAV_PLAYER_BUFFER_LEN];

static uint32_t data_remain = -1;

void UAL_WAV_PLAYER_Init() {
	PLAYER_SEMAPHORE_ID = osSemaphoreNew(1U, 0U, NULL);
}

UAL_STATUS_t UAL_WAV_PLAYER_ValidateMetadata(const WAV_FILE_METADATA_t *metadata) {
	if (metadata->audio_format != 1) {
		return UAL_STATUS_ERROR;
	}

	if (metadata->channels_num != 2) {
		return UAL_STATUS_ERROR;
	}

	if (metadata->sample_rate != 44100) {
		return UAL_STATUS_ERROR;
	}

	if (metadata->bytes_per_sample != 4) {
		return UAL_STATUS_ERROR;
	}

	if (metadata->bits_per_sample_per_channel != 16) {
		return UAL_STATUS_ERROR;
	}

	// 44100 * 2 * 2
	uint32_t expected_bytes_per_second = 176400;
	if (metadata->bytes_per_second != expected_bytes_per_second) {
		return UAL_STATUS_ERROR;
	}

	if (metadata->data_size < sizeof(signal_buff1) + sizeof(signal_buff2)) {
		return UAL_STATUS_ERROR;
	}

	return UAL_STATUS_OK;
}

UAL_STATUS_t UAL_WAV_PLAYER_Play(const char *fname, const uint64_t data_section_pointer, const uint32_t data_size) {
	FIL file;
	FRESULT fres;
	unsigned int bytesRead;

	fres = f_open(&file, fname, FA_READ);
	if (fres != FR_OK) {
		return UAL_STATUS_ERROR;
	}

	fres = f_lseek(&file, data_section_pointer);
	if (fres != FR_OK) {
		f_close(&file);
		return UAL_STATUS_ERROR;
	}

	fres = f_read(&file, (uint8_t*) signal_buff1, sizeof(signal_buff1),
			&bytesRead);
	if (fres != FR_OK) {
		f_close(&file);
		return UAL_STATUS_ERROR;
	}

	fres = f_read(&file, (uint8_t*) signal_buff2, sizeof(signal_buff2),
			&bytesRead);
	if (fres != FR_OK) {
		f_close(&file);
		return UAL_STATUS_ERROR;
	}

	continue_i2s_transition = 1;
	signal_play_buff = signal_buff1;
	signal_read_buff = signal_buff2;

	UAL_STATUS_t res;
	res = UAL_I2S_UTILS_TRANSMIT((uint16_t*) signal_buff1, UAL_WAV_PLAYER_BUFFER_LEN);
	if (res != UAL_STATUS_OK) {
		f_close(&file);
		return UAL_STATUS_ERROR;
	}

	if (data_section_pointer >= data_size) {
		data_remain = 0;
	} else {
		data_remain = data_size - data_section_pointer;
	}

	uint32_t timeout = pdMS_TO_TICKS(10000);
	osStatus_t status;

	while (data_remain >= sizeof(signal_buff1)) {
		status = osSemaphoreAcquire(PLAYER_SEMAPHORE_ID, timeout);
		if (status != osOK && status != osErrorTimeout) {
			UAL_Error_Handler();
		}

		if (continue_i2s_transition == 0) {
			break;
		}

		res = f_read(&file, (uint8_t*) signal_read_buff, sizeof(signal_buff1),
				&bytesRead);
		if (fres != FR_OK) {
			f_close(&file);
			return UAL_STATUS_ERROR;
		}

		data_remain -= sizeof(signal_buff1);
	}

	continue_i2s_transition = 0;

	res = f_close(&file);

	return UAL_STATUS_OK;
}

uint32_t UAL_WAV_PLAYER_GetRemainData() {
	return data_remain;
}

void UAL_WAV_PLAYER_Stop() {
	continue_i2s_transition = 0;
}

