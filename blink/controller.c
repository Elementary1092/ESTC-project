#include "controller.h"
#include <malloc.h>

void init_time_controller(struct time_controller_t *c, uint32_t sequence)
{
	c->a = malloc(18 * sizeof(uint8_t));
	c->len = 0;
	c->index = 0;

	for (size_t i = 0; sequence != 0; i++)
	{
		c->a[i] = (uint8_t)sequence % 10;
		sequence /= 10;
		c->len++;
	}

	for (size_t i = 0; i < c->len / 2; i++)
	{
		size_t j = c->len - i - 1;
		uint8_t temp = c->a[i];
		c->a[i] = c->a[j];
		c->a[j] = temp;
	}
}

uint8_t next(struct time_controller_t *c)
{
	if (c->index >= c->len)
	{
		c->index = 0;
	}

	return c->a[c->index++];
}

void destruct_time_controller(struct time_controller_t *c)
{
	c->len = 0;
	c->index = 0;
	free(c->a);
}