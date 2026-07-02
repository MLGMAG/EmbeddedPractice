#ifndef __PLAYLIST_MANAGER_H
#define __PLAYLIST_MANAGER_H

#include "commons.h"
#include "stdint.h"

UAL_STATUS_t UAL_PLAYLIST_MANAGER_Init();

UAL_TRACK_t* UAL_PLAYLIST_MANAGER_GetTracks();

uint8_t UAL_PLAYLIST_MANAGER_GetTrackCount();

UAL_TRACK_t* UAL_PLAYLIST_MANAGER_GetCurrentTrack();

uint8_t UAL_PLAYLIST_MANAGER_GetCurrentTrackIndex();

UAL_TRACK_t* UAL_PLAYLIST_MANAGER_NextTrack();

#endif /* __PLAYLIST_MANAGER_H */
