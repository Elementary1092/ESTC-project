#include <hal/nrf_gpio.h>
#include <malloc.h>
#include "led_controller.h"

#define SWITCH_LED_ON_SIGNAL 0
#define SWITCH_LED_OFF_SIGNAL 1

void init_led_controller(struct led_controller_t *c, const char *sequence, uint8_t nseq)
{
	NRFX_ASSERT(c != NULL);
	NRFX_ASSERT(sequence != NULL);

	/* Getting gpio pin number of red, green and blue leds */
	uint32_t red = NRF_GPIO_PIN_MAP(0, 8);
	uint32_t green = NRF_GPIO_PIN_MAP(1, 9);
	uint32_t blue = NRF_GPIO_PIN_MAP(0, 12);

	/* Configuring these pins as output */
	nrf_gpio_cfg_output(red);
	nrf_gpio_cfg_output(green);
	nrf_gpio_cfg_output(blue);

	/* Switching off leds */
	nrf_gpio_pin_write(red, SWITCH_LED_OFF_SIGNAL);
	nrf_gpio_pin_write(green, SWITCH_LED_OFF_SIGNAL);
	nrf_gpio_pin_write(blue, SWITCH_LED_OFF_SIGNAL);

	/* Initializing leds blink sequence */
	c->gpio_pin_sequence = malloc(((size_t)nseq) * sizeof(uint32_t));
	c->pin_idx = 0;
	uint8_t sequence_last_idx = 0;
	for (uint8_t i = 0; i < nseq; i++)
	{
		switch (sequence[i])
		{
		case 'R':
			c->gpio_pin_sequence[sequence_last_idx++] = red;
			break;
		case 'G':
			c->gpio_pin_sequence[sequence_last_idx++] = green;
			break;
		case 'B':
			c->gpio_pin_sequence[sequence_last_idx++] = blue;
			break;
		}
	}
	c->sequence_size = sequence_last_idx;
}

void switch_on_next_led(struct led_controller_t *c)
{
	if (c->pin_idx >= c->sequence_size)
	{
		c->pin_idx = 0;
	}

	nrf_gpio_pin_write(c->gpio_pin_sequence[c->pin_idx++], SWITCH_LED_ON_SIGNAL);
}

void switch_off_led(struct led_controller_t *c)
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

void free_led_controller(struct led_controller_t *c)
{
	free(c->gpio_pin_sequence);
	c->pin_idx = 0;
	c->sequence_size = 0;
}