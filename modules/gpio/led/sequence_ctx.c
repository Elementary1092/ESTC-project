#include <hal/nrf_gpio.h>
#include <string.h>
#include "sequence_ctx.h"
#include "led.h"

void led_init_ctx(led_sequence_ctx_t *c, uint32_t *blink_queue, const char *sequence)
{
	NRFX_ASSERT(c != NULL);
	NRFX_ASSERT(sequence != NULL);

	led_init_all();

	/* Initializing leds blink sequence */
	c->gpio_pin_sequence = blink_queue;
	c->next_pin_idx = 0;
	c->curr_pin_idx = 0;
	uint8_t sequence_last_idx = 0;
	size_t nseq = BLINK_SEQUENCE_MAX_SIZE < strlen(sequence) ? BLINK_SEQUENCE_MAX_SIZE : ((size_t)nseq);
	for (uint8_t i = 0; i < nseq; i++)
	{
		switch (sequence[i])
		{
		case 'R':
			c->gpio_pin_sequence[sequence_last_idx++] = LED_PCA10059_RED;
			break;
		case 'G':
			c->gpio_pin_sequence[sequence_last_idx++] = LED_PCA10059_GREEN;
			break;
		case 'B':
			c->gpio_pin_sequence[sequence_last_idx++] = LED_PCA10059_BLUE;
			break;
		case 'Y':
			c->gpio_pin_sequence[sequence_last_idx++] = LED_PCA10059_YELLOW;
			break;
		}
	}
	c->sequence_size = sequence_last_idx;
}

void led_switch_on_next(led_sequence_ctx_t *c)
{
	if (c->next_pin_idx >= c->sequence_size)
	{
		c->next_pin_idx = 0;
	}
	c->curr_pin_idx = c->next_pin_idx;

	nrf_gpio_pin_write(c->gpio_pin_sequence[c->next_pin_idx++], LED_SWITCH_ON_SIGNAL);
}

void led_switch_on_current(led_sequence_ctx_t *c)
{
	nrf_gpio_pin_write(c->gpio_pin_sequence[c->curr_pin_idx], LED_SWITCH_ON_SIGNAL);
}

void led_switch_off(led_sequence_ctx_t *c)
{
	nrf_gpio_pin_write(c->gpio_pin_sequence[c->curr_pin_idx], LED_SWITCH_OFF_SIGNAL);
}
