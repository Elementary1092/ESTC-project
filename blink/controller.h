#include <stdint.h>
#include <unistd.h>

#ifndef BLINK_TIME_CONTROLLER_H
#define BLINK_TIME_CONTROLLER_H

#define BLINK_QUEUE_MAX_SIZE 9

struct time_controller_t
{
	uint8_t *a;
	size_t len;
	size_t index;
};

void init_time_controller(struct time_controller_t *controller, uint8_t *sequence_queue, uint32_t sequence);
uint8_t next(struct time_controller_t *controller);
void destruct_time_controller(struct time_controller_t *controller);

#endif