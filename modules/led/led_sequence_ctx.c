#include <hal/nrf_gpio.h>
#include <string.h>
#include "led_sequence_ctx.h"

#define SWITCH_LED_ON_SIGNAL 0
#define SWITCH_LED_OFF_SIGNAL 1

#define LED_PCA10059_RED   NRF_GPIO_PIN_MAP(0, 8)
#define LED_PCA10059_GREEN NRF_GPIO_PIN_MAP(1, 9)
#define LED_PCA10059_BLUE  NRF_GPIO_PIN_MAP(0, 12)

void led_init_ctx(led_sequence_ctx_t *c, uint32_t *blink_queue, const char *sequence)
{
	NRFX_ASSERT(c != NULL);
	NRFX_ASSERT(sequence != NULL);

	/* Configuring these pins as output */
	nrf_gpio_cfg_output(LED_PCA10059_RED);
	nrf_gpio_cfg_output(LED_PCA10059_GREEN);
	nrf_gpio_cfg_output(LED_PCA10059_BLUE);

	/* Switching off leds */
	nrf_gpio_pin_write(LED_PCA10059_RED, SWITCH_LED_OFF_SIGNAL);
	nrf_gpio_pin_write(LED_PCA10059_GREEN, SWITCH_LED_OFF_SIGNAL);
	nrf_gpio_pin_write(LED_PCA10059_BLUE, SWITCH_LED_OFF_SIGNAL);

	/* Initializing leds blink sequence */
	c->gpio_pin_sequence = blink_queue;
	c->pin_idx = 0;
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
		}
	}
	c->sequence_size = sequence_last_idx;
}

void led_switch_on_next(led_sequence_ctx_t *c)
{
	if (c->pin_idx >= c->sequence_size)
	{
		c->pin_idx = 0;
	}

	nrf_gpio_pin_write(c->gpio_pin_sequence[c->pin_idx++], SWITCH_LED_ON_SIGNAL);
}

void led_switch_off(led_sequence_ctx_t *c)
{
	uint32_t switch_off_pin_idx = c->pin_idx - 1;
	if (c->pin_idx == 0 && c->sequence_size != 0)
	{
		switch_off_pin_idx = c->sequence_size - 1;
	}
	else if (c->sequence_size == 0)
	{
		switch_off_pin_idx = 0;
	}

	nrf_gpio_pin_write(c->gpio_pin_sequence[switch_off_pin_idx], SWITCH_LED_OFF_SIGNAL);
}
