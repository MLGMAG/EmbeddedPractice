#include "player_task.h"
#include "cmsis_os.h"
#include "wav_player.h"

extern osMessageQueueId_t PLAYER_QUEUE;
extern osMessageQueueId_t MANAGER_QUEUE;

static void init() {
	UAL_WAV_PLAYER_Init();
}

void UAL_PLAYER_TASK_Start(void *argument) {
	init();

	osStatus_t status;
	UAL_TRACK_MSG_t track_msg;
	UAL_MANAGER_MSG_t manager_msg;

	uint32_t getMessage_delay = pdMS_TO_TICKS(10000);

	while (1) {
		status = osMessageQueueGet(PLAYER_QUEUE, &track_msg, NULL,
				getMessage_delay);

		if (status == osOK) {
			UAL_WAV_PLAYER_Play(track_msg.path, track_msg.data_section_pointer,
					track_msg.file_size);

			uint32_t data_remain = UAL_WAV_PLAYER_GetRemainData();
			if (data_remain < 4096) {
				manager_msg.type = UAL_MANAGER_MSG_TYPE_TRACK_FINISH;
				status = osMessageQueuePut(MANAGER_QUEUE, &manager_msg, 0U,
						pdMS_TO_TICKS(100));
				if (status != osOK) {
					UAL_Error_Handler();
				}
			}
		}

		if (status != osOK && status != osErrorTimeout) {
			UAL_Error_Handler();
		}

		osDelay(1);
	}
}
