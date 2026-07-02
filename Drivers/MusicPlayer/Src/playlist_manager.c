#include "playlist_manager.h"
#include "fatfs.h"
#include <string.h>
#include <stdio.h>
#include "wav_utils.h"
#include "wav_player.h"

#define PLAYLIST_MAX_LEN 10
#define PLAYLIST_DIR "music"
#define WAV_ENDING ".wav"

static UAL_TRACK_t tracks[PLAYLIST_MAX_LEN];
static uint8_t tracks_count = 0;
static uint8_t current_track_index = -1;

static uint8_t filename_has_ending(char *filename, char *ending);
static UAL_STATUS_t read_tracks();

UAL_STATUS_t UAL_PLAYLIST_MANAGER_Init() {
	UAL_STATUS_t status;

	status = read_tracks();
	if (status != UAL_STATUS_OK) {
		return status;
	}

	if (tracks_count > 0) {
		current_track_index = 0;
	}

	return UAL_STATUS_OK;
}

UAL_TRACK_t* UAL_PLAYLIST_MANAGER_GetTracks() {
	return tracks;
}

uint8_t UAL_PLAYLIST_MANAGER_GetTrackCount() {
	return tracks_count;
}

UAL_TRACK_t* UAL_PLAYLIST_MANAGER_GetCurrentTrack() {
	if (current_track_index == -1) {
		return NULL;
	}
	return &tracks[current_track_index];
}

uint8_t UAL_PLAYLIST_MANAGER_GetCurrentTrackIndex() {
	return current_track_index;
}

UAL_TRACK_t* UAL_PLAYLIST_MANAGER_NextTrack() {
	if (current_track_index + 1 != tracks_count) {
		current_track_index++;
	} else {
		current_track_index = 0;
	}

	return UAL_PLAYLIST_MANAGER_GetCurrentTrack();
}

static uint8_t filename_has_ending(char *filename, char *ending) {
	uint16_t ending_len = strlen(ending);

	uint16_t filename_len = strlen(filename);
	if (ending_len >= filename_len) {
		return 0;
	}

	if (strcmp(filename + filename_len - ending_len, ending) != 0) {
		return 0;
	}

	return 1;
}

static UAL_STATUS_t read_tracks() {
	FRESULT res;
	DIR dir;
	FILINFO fno;

	res = f_opendir(&dir, PLAYLIST_DIR);
	if (res != FR_OK) {
//        printf("Failed to open directory. Error: %d\n", res);
		return UAL_STATUS_ERROR;
	}

	while (tracks_count != PLAYLIST_MAX_LEN) {
		res = f_readdir(&dir, &fno);
		if (res != FR_OK || fno.fname[0] == 0) {
			break;
		}

		if (fno.fname[0] == '.') {
			continue;
		}

		if (!(fno.fattrib & AM_DIR)) {
			uint8_t is_wav_file = filename_has_ending(fno.fname,
			WAV_ENDING);
			if (is_wav_file == 1) {
				char *path = tracks[tracks_count].path;
				sprintf(path, "%s/%s", PLAYLIST_DIR, fno.fname);
				UAL_WAV_UTILS_EXTRACT_DETAILS_t details =
						UAL_WAV_FILE_ExtractMetadata(path);
				if (details.status != UAL_STATUS_OK) {
					continue;
				}

				if (UAL_WAV_PLAYER_ValidateMetadata(&details.metadata)
						!= UAL_STATUS_OK) {
					continue;
				}

				memcpy(&tracks[tracks_count].metadata, &details.metadata,
						sizeof(details.metadata));
				strcpy(tracks[tracks_count].filename, fno.fname);
				tracks_count++;
			}
		}
	}

	return UAL_STATUS_OK;
}

