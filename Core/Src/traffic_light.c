#include "traffic_light.h"

void UAL_TRAFFIC_LIGHT_init(UAL_TRAFFIC_LIGHT_t* data) {
	data->turn_off_pin(&(data->red_pin));
	data->turn_off_pin(&(data->yellow_pin));
	data->turn_off_pin(&(data->green_pin));

	UAL_TRAFFIC_LIGHT_LIGHT_STATE state = data->state;
	if (state == RED) {
		data->turn_on_pin(&(data->red_pin));
	}

	if (state == YELLOW) {
		data->turn_on_pin(&(data->yellow_pin));
	}

	if (state == GREEN) {
		data->turn_on_pin(&(data->green_pin));
	}
}

void UAL_TRAFFIC_LIGHT_handle(UAL_TRAFFIC_LIGHT_t* data) {
	UAL_TRAFFIC_LIGHT_LIGHT_STATE state = data->state;

	if (state == RED) {
		data->turn_off_pin(&(data->red_pin));
		data->turn_on_pin(&(data->yellow_pin));
		data->state = YELLOW;
	}

	if (state == YELLOW) {
		data->turn_off_pin(&(data->yellow_pin));
		data->turn_on_pin(&(data->green_pin));
		data->state = GREEN;
	}

	if (state == GREEN) {
		data->turn_off_pin(&(data->green_pin));
		data->turn_on_pin(&(data->red_pin));
		data->state = RED;
	}
}
