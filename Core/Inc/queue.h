#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>

typedef struct {
	uint8_t element_size;
	uint8_t len;
	void **elements;
	int first_element_index;
	int last_element_index;
} QUEUE_DATA_t;

void QUEUE_init(QUEUE_DATA_t*, const uint8_t, const uint8_t, void (*) (void));

void QUEUE_deinit(QUEUE_DATA_t*);

uint8_t QUEUE_size_get(const QUEUE_DATA_t*);

uint8_t QUEUE_push(QUEUE_DATA_t*, void*);

void* QUEUE_pop(QUEUE_DATA_t*);

void* QUEUE_peek(QUEUE_DATA_t*);

#endif // __QUEUE_H__
