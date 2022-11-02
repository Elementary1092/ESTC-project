#include "controller.h"
#include <malloc.h>

void blink_time_controller_init(struct time_controller_t *c, uint8_t *blink_queue, uint32_t sequence)
{
	c->a = blink_queue;
	c->len = 0;
	c->index = 0;

	for (size_t i = 0; sequence != 0; i++)
	{
		c->a[i] = (uint8_t)(sequence % 10);
		sequence /= 10;
		(c->len)++;
	}
}

uint8_t blink_time_controller_next(struct time_controller_t *c)
{
	if (c->index >= c->len)
	{
		c->index = 0;
	}

	return c->a[(c->index)++];
}
