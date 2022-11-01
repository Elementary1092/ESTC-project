#include <hal/nrf_gpio.h>
#include "button_controller.h"

void init_button_controller(button_controller_t *c, int port, int pin)
{
	NRFX_ASSERT(c != NULL);
	NRFX_ASSERT(port >= 0 && port < 2);
	NRFX_ASSERT(pin >= 0 && port < 32);
	
	c->gpio_pin = NRF_GPIO_PIN_MAP(port, pin);

	nrf_gpio_cfg_input(c->gpio_pin, NRF_GPIO_PIN_PULLDOWN);
}

uint32_t is_pressed(button_controller_t *c)
{
	NRFX_ASSERT(c != NULL);
	return nrf_gpio_pin_read(c->gpio_pin);
}
