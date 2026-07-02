#include "button_state.h"

#define MAX_RECORDING_COUNT 5

static UAL_BUTTON_STATE_t state = UAL_BUTTON_STATE_IDLE;

static uint8_t recording_count = 0;
static uint8_t recordings[MAX_RECORDING_COUNT];

UAL_BUTTON_STATE_t UAL_BUTTON_STATE_GetState() {
	return state;
}

void UAL_BUTTON_STATE_UpdateState(uint8_t value) {
	if (value == 1 && state == UAL_BUTTON_STATE_IDLE) {
		state = UAL_BUTTON_STATE_PRESSED;
	}

	if (state
			== UAL_BUTTON_STATE_PRESSED&& recording_count < MAX_RECORDING_COUNT) {
		recordings[recording_count] = value;
		recording_count++;
	}

	if (state
			== UAL_BUTTON_STATE_PRESSED&& recording_count == MAX_RECORDING_COUNT) {
		for (int i = 0; i < MAX_RECORDING_COUNT; i++) {
			if (recordings[i] == 0) {
				state = UAL_BUTTON_STATE_IDLE;
				recording_count = 0;
				break;
			}

			if (i == MAX_RECORDING_COUNT - 1) {
				state = UAL_BUTTON_STATE_HELD;
				recording_count = 0;
			}
		}
	}

	if (state == UAL_BUTTON_STATE_RELEASE) {
		state = UAL_BUTTON_STATE_IDLE;
	}

	if (state
			== UAL_BUTTON_STATE_HELD&& recording_count < MAX_RECORDING_COUNT) {
		recordings[recording_count] = value;
		recording_count++;
	}

	if (state
			== UAL_BUTTON_STATE_HELD&& recording_count == MAX_RECORDING_COUNT) {
		for (int i = 0; i < MAX_RECORDING_COUNT; i++) {
			if (recordings[i] == 1) {
				state = UAL_BUTTON_STATE_HELD;
				recording_count = 0;
				break;
			}

			if (i == MAX_RECORDING_COUNT - 1) {
				state = UAL_BUTTON_STATE_RELEASE;
				recording_count = 0;
			}
		}
	}
}
