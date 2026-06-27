#ifndef __TRAFFICLIGHT_H__
#define __TRAFFICLIGHT_H__

#include <stdint.h>

typedef enum {
	RED, YELLOW, GREEN
} UAL_TRAFFIC_LIGHT_LIGHT_STATE;

typedef struct {
	void* port;
	uint16_t pin_number;
} UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t;

typedef struct {
	UAL_TRAFFIC_LIGHT_LIGHT_STATE state;
	UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t red_pin;
	UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t yellow_pin;
	UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t green_pin;
	void (*turn_off_pin)(UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t*);
	void (*turn_on_pin)(UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t*);
} UAL_TRAFFIC_LIGHT_t;

void UAL_TRAFFIC_LIGHT_init(UAL_TRAFFIC_LIGHT_t*);

void UAL_TRAFFIC_LIGHT_handle(UAL_TRAFFIC_LIGHT_t*);

#endif // __TRAFFICLIGHT_H__
