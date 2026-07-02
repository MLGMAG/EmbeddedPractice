#include "display_manager.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "string.h"
#include "stdio.h"

#define MAX_LINE_LEN 20
#define MAX_LINES_COUNT 4

static uint8_t screen_message_buffer[MAX_LINE_LEN];

void UAL_DISPLAY_MANAGER_Init() {
	ssd1306_Init();
}

UAL_STATUS_t UAL_DISPLAY_MANAGER_DisplayLines(char **lines, uint8_t count) {
	if (count > MAX_LINES_COUNT) {
		return UAL_STATUS_ERROR;
	}

	ssd1306_Fill(Black);
	uint8_t height = 5;
	for (int i = 0; i < count; i++) {
		uint32_t line_len = strlen(lines[i]);
		if (line_len > MAX_LINE_LEN) {
			return UAL_STATUS_ERROR;
		}
		strcpy((char*) screen_message_buffer, lines[i]);
		ssd1306_SetCursor(5, height);
		ssd1306_WriteString((char*) screen_message_buffer, Font_6x8, White);
		height += 15;
	}

	ssd1306_UpdateScreen();

	return UAL_STATUS_OK;
}

UAL_STATUS_t UAL_DISPLAY_MANAGER_DisplayPlaylist(UAL_TRACK_t *tracks,
		uint8_t count, uint8_t start_index) {
	uint8_t limit = count;

	if (count == 0) {
		limit = 2;
	}

	if (count > MAX_LINES_COUNT) {
		limit = MAX_LINES_COUNT;
	}

	uint8_t buffer_size = limit != MAX_LINES_COUNT ? limit + 1 : limit;
	uint8_t buffer_index = 0;
	if (count == 0) {
		buffer_size = 2;
	}

	char local_buffers[buffer_size][MAX_LINE_LEN + 1];
	char *line_pointers[buffer_size];

	memcpy(local_buffers[buffer_index], UAL_DISPLAY_PLAYLIST_TITLE, MAX_LINE_LEN);
	local_buffers[buffer_index][MAX_LINE_LEN] = '\0';
	line_pointers[buffer_index] = local_buffers[buffer_index];
	buffer_index++;

	uint8_t track_index = start_index;
	if (count != 0) {
		for (int i = 0; i < limit && buffer_index < buffer_size; i++) {
			char *filename = tracks[i].filename;
			sprintf(local_buffers[buffer_index], "%d. %s", track_index, filename);
			local_buffers[buffer_index][MAX_LINE_LEN] = '\0';
			line_pointers[buffer_index] = local_buffers[buffer_index];
			buffer_index++;
			track_index++;
		}
	} else {
		memcpy(local_buffers[buffer_index], UAL_DISPLAY_NO_TRACKS, MAX_LINE_LEN);
		local_buffers[buffer_index][MAX_LINE_LEN] = '\0';
		line_pointers[buffer_index] = local_buffers[buffer_index];
	}

	return UAL_DISPLAY_MANAGER_DisplayLines(line_pointers, buffer_size);
}

