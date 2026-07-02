#ifndef __DIPLAY_MANAGER_H
#define __DISPLAY_MANAGER_H

#include "commons.h"

#define UAL_DISPLAY_MANAGER_MAX_LINE_LEN 20

void UAL_DISPLAY_MANAGER_Init();

UAL_STATUS_t UAL_DISPLAY_MANAGER_DisplayLines(char** lines, uint8_t count);

UAL_STATUS_t UAL_DISPLAY_MANAGER_DisplayPlaylist(UAL_TRACK_t *tracks,
		uint8_t count, uint8_t start_index);

#endif /* __DISPLAY_MANAGER_H */
