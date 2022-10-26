#include <stdint.h>
#include <unistd.h>

#ifndef BLINK_TIME_CONTROLLER_H
#define BLINK_TIME_CONTROLLER_H

struct time_controller_t
{
	uint8_t *a;
	size_t len;
	size_t index;
};

void init_time_controller(struct time_controller_t *, uint32_t);
uint8_t next(struct time_controller_t *);
void destruct_time_controller(struct time_controller_t *);

#endif