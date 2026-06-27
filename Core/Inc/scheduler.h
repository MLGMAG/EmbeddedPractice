#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "queue.h"

typedef struct {
	void (*task_func)(void);
	uint32_t delay_ms;
} UAL_SCHEDULER_TASK_t;

typedef struct {
	UAL_SCHEDULER_TASK_t* tasks;
	uint16_t tasks_count;
	QUEUE_DATA_t* tasks_queue;
	uint32_t last_execution_time_ms;
	void (*default_task) (void);
	uint32_t (*get_current_time_ms) (void);
} UAL_SCHEDULER_t;

void UAL_SCHEDULER_Init(UAL_SCHEDULER_t*, void (*)(void));

void UAL_SCHEDULER_Start(UAL_SCHEDULER_t*);

#endif // __SCHEDULER_H__
