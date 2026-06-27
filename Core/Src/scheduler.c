#include <scheduler.h>
#include <stdlib.h>

void UAL_SCHEDULER_Init(UAL_SCHEDULER_t* scheduler, void (*error_handler)(void)) {
	QUEUE_init(scheduler->tasks_queue, 5, sizeof(UAL_SCHEDULER_TASK_t*), error_handler);

	for (int i = 0; i < scheduler->tasks_count; i++) {
		QUEUE_push(scheduler->tasks_queue, (void*) &(scheduler->tasks[i]));
	}

	scheduler->last_execution_time_ms = scheduler->get_current_time_ms();
}

void UAL_SCHEDULER_Start(UAL_SCHEDULER_t* scheduler) {
	while(1) {
		uint32_t current_time_ms = scheduler->get_current_time_ms();
		uint32_t delta_time_ms = current_time_ms - scheduler->last_execution_time_ms;
		UAL_SCHEDULER_TASK_t *task = (UAL_SCHEDULER_TASK_t*) QUEUE_peek(scheduler->tasks_queue);

		if (task != NULL && delta_time_ms >= task->delay_ms) {
			QUEUE_pop(scheduler->tasks_queue);
			task->task_func();
			QUEUE_push(scheduler->tasks_queue, task);
			scheduler->last_execution_time_ms = current_time_ms;
		} else {
			scheduler->default_task();
		}
	}
}
