#include <stdlib.h>
#include "queue.h"

void QUEUE_init(QUEUE_DATA_t *queue, const uint8_t queue_len,
		const uint8_t queue_element_size, void (*error_handler)(void)) {
	uint8_t queue_allocation_size = queue_len * queue_element_size;

	if (queue_len > 5 || queue_len <= 0 || queue_element_size > 10
			|| queue_element_size <= 0) {
		error_handler();
	}

	queue->element_size = queue_element_size;
	queue->len = queue_len;
	queue->first_element_index = 0;
	queue->last_element_index = -1;
	queue->elements = (void**) malloc(queue_allocation_size);
	if (queue->elements == NULL) {
		error_handler();
	}
}

void QUEUE_deinit(QUEUE_DATA_t *queue) {
	queue->element_size = 0;
	queue->len = 0;
	queue->first_element_index = -1;
	queue->last_element_index = -1;

	free(queue->elements);
	queue->elements = NULL;
}

uint8_t QUEUE_size_get(const QUEUE_DATA_t *queue) {
	if (queue->last_element_index == -1) {
		return 0;
	} else if (queue->first_element_index > queue->last_element_index) {
		return queue->len - queue->first_element_index
				+ queue->last_element_index + 1;
	} else {
		return queue->last_element_index - queue->first_element_index + 1;
	}
}

uint8_t QUEUE_push(QUEUE_DATA_t *queue, void *element) {
	uint8_t queue_size = QUEUE_size_get(queue);

	if (queue_size == queue->len) {
		return 0;
	}

	if (queue_size == 0) {
		queue->last_element_index = queue->first_element_index - 1;
	}

	if (queue->last_element_index + 1 == queue->len) {
		queue->last_element_index = 0;
	} else {
		queue->last_element_index = queue->last_element_index + 1;
	}

	queue->elements[queue->last_element_index] = element;
	return 1;
}

void* QUEUE_pop(QUEUE_DATA_t *queue) {
	uint8_t queue_size = QUEUE_size_get(queue);
	if (queue_size == 0) {
		return NULL;
	}

	void *element = queue->elements[queue->first_element_index];
	if (queue_size == 1) {
		queue->last_element_index = -1;
	} else if (queue->first_element_index + 1 == queue->len) {
		queue->first_element_index = 0;
	} else {
		queue->first_element_index = queue->first_element_index + 1;
	}

	return element;
}

void* QUEUE_peek(QUEUE_DATA_t *queue) {
	uint8_t queue_size = QUEUE_size_get(queue);
	if (queue_size == 0) {
		return NULL;
	}

	return queue->elements[queue->first_element_index];
}
