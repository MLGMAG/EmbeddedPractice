#include "manager_task.h"
#include "fatfs_utils.h"
#include "display_manager.h"
#include "playlist_manager.h"
#include "wav_player.h"
#include <string.h>

extern osMessageQueueId_t PLAYER_QUEUE;
extern osMessageQueueId_t MANAGER_QUEUE;

static FATFS fatfs;
static uint8_t player_stop = 1;
static uint32_t last_button_press_ms = 0;
static uint32_t button_press = 0;
static uint32_t button_double_press = 0;

static void init(void);
static void send_track_to_queue(UAL_TRACK_t *track);
static void handle_track_finish(void);
static void handle_pause(void);
static void handle_skip(void);
static uint32_t get_time_ms(void);

void UAL_MANAGER_TASK_Start(void *argument) {
	init();

	osStatus_t status;
	UAL_MANAGER_MSG_t manager_msg;

	uint32_t get_timeout = pdMS_TO_TICKS(100);
	while (1) {
		status = osMessageQueueGet(MANAGER_QUEUE, &manager_msg, NULL,
				get_timeout);

		if (status == osOK) {
			if (manager_msg.type == UAL_MANAGER_MSG_TYPE_TRACK_FINISH) {
				handle_track_finish();
			}
			if (manager_msg.type == UAL_MANAGER_MSG_TYPE_BUTTON_PRESS) {
				last_button_press_ms = get_time_ms();
				if (button_press == 1) {
					button_double_press = 1;
				}
				button_press = 1;
			}
		}

		if (status != osOK && status != osErrorTimeout) {
			UAL_Error_Handler();
		}

		uint32_t delta_ms = get_time_ms() - last_button_press_ms;
		if (button_press == 1 && delta_ms > 300) {
			if (button_double_press == 1) {
				handle_skip();
			} else {
				handle_pause();
			}

			button_press = 0;
			button_double_press = 0;
		}
		osDelay(1);
	}
}

static void init() {
	while (UAL_FATFS_UTILITY_MountDrive(&fatfs) != UAL_FATFS_STATUS_OK) {
		osDelay(pdMS_TO_TICKS(1000));
	}

	if (UAL_PLAYLIST_MANAGER_Init() != UAL_STATUS_OK) {
		UAL_Error_Handler();
	}

	UAL_DISPLAY_MANAGER_Init();

	UAL_TRACK_t *tracks = UAL_PLAYLIST_MANAGER_GetTracks();
	uint8_t tracks_count = UAL_PLAYLIST_MANAGER_GetTrackCount();
	UAL_DISPLAY_MANAGER_DisplayPlaylist(tracks, tracks_count, 1);
}

static void handle_track_finish() {
	UAL_TRACK_t *track_to_play = UAL_PLAYLIST_MANAGER_NextTrack();

	UAL_TRACK_t *tracks = UAL_PLAYLIST_MANAGER_GetTracks();
	uint8_t index = UAL_PLAYLIST_MANAGER_GetCurrentTrackIndex();
	uint8_t tracks_count = UAL_PLAYLIST_MANAGER_GetTrackCount() - index;
	UAL_DISPLAY_MANAGER_DisplayPlaylist(&(tracks[index]), tracks_count,
			index + 1);

	send_track_to_queue(track_to_play);
}

static void handle_pause() {
	if (player_stop == 0) {
		UAL_WAV_PLAYER_Stop();
		player_stop = 1;
	} else {
		UAL_TRACK_t *track = UAL_PLAYLIST_MANAGER_GetCurrentTrack();
		if (track == NULL) {
			UAL_Error_Handler();
		}

		osStatus_t status;
		UAL_TRACK_MSG_t track_msg;

		uint32_t remain_data = UAL_WAV_PLAYER_GetRemainData();
		uint32_t new_pointer = 0;
		if (remain_data == -1) {
			new_pointer = 0;
		} else {
			new_pointer = track->metadata.file_size - remain_data;
		}

		strcpy(track_msg.path, track->path);
		track_msg.file_size = track->metadata.file_size;
		track_msg.data_section_pointer = new_pointer;

		status = osMessageQueuePut(PLAYER_QUEUE, &track_msg, 0U,
				pdMS_TO_TICKS(100));
		if (status != osOK) {
			UAL_Error_Handler();
		}

		player_stop = 0;
	}
}

static void handle_skip() {
	UAL_WAV_PLAYER_Stop();
	player_stop = 0;
	handle_track_finish();
}

static void send_track_to_queue(UAL_TRACK_t *track) {
	osStatus_t status;
	UAL_TRACK_MSG_t track_msg;

	strcpy(track_msg.path, track->path);
	track_msg.file_size = track->metadata.file_size;
	track_msg.data_section_pointer = track->metadata.data_section_pointer;

	status = osMessageQueuePut(PLAYER_QUEUE, &track_msg, 0U,
			pdMS_TO_TICKS(100));
	if (status != osOK) {
		UAL_Error_Handler();
	}
}

static uint32_t get_time_ms() {
	uint32_t current_ticks = osKernelGetTickCount();
	uint32_t tick_freq = osKernelGetTickFreq();
	return (current_ticks * 1000) / tick_freq;
}
