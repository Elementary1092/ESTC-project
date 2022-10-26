#include <stdint.h>
#include <unistd.h>

#ifndef BLINK_TIME_CONTROLLER_H
#define BLINK_TIME_CONTROLLER_H

#define BOARD_ID 7198

struct controller_t
{
	uint8_t *a;
	size_t len;
	size_t index;
};

void init_time_controller(struct controller_t *, uint32_t);
uint8_t next(struct controller_t *);
void destruct_time_controller(struct controller_t *);

#endif