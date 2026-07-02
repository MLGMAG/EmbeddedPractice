#include "i2s_utils.h"
#include "stm32f4xx_hal.h"
#include "wav_player.h"
#include "cmsis_os.h"

extern osSemaphoreId_t PLAYER_SEMAPHORE_ID;

extern I2S_HandleTypeDef hi2s1;

extern volatile uint8_t continue_i2s_transition;
extern volatile uint16_t signal_buff1[UAL_WAV_PLAYER_BUFFER_LEN];
extern volatile uint16_t signal_buff2[UAL_WAV_PLAYER_BUFFER_LEN];
extern volatile uint16_t *signal_play_buff;
extern volatile uint16_t *signal_read_buff;

UAL_STATUS_t UAL_I2S_UTILS_TRANSMIT(uint16_t *data, uint16_t len) {
	HAL_StatusTypeDef status = HAL_I2S_Transmit_IT(&hi2s1, data, len);

	if (status != HAL_OK) {
		return UAL_STATUS_ERROR;
	}

	return UAL_STATUS_OK;
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (continue_i2s_transition == 0) {
		osSemaphoreRelease(PLAYER_SEMAPHORE_ID);
		return;
	}

	volatile uint16_t *temp = signal_play_buff;
	signal_play_buff = signal_read_buff;
	signal_read_buff = temp;

	HAL_I2S_Transmit_IT(&hi2s1, (uint16_t*) signal_play_buff, UAL_WAV_PLAYER_BUFFER_LEN);

	osSemaphoreRelease(PLAYER_SEMAPHORE_ID);
}
