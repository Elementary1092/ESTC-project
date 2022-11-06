#include <hal/nrf_gpio.h>
#include "board_button.h"

void button_init(board_button_t button)
{
	nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP);
}

uint32_t button_is_pressed(board_button_t button)
{
	return !(nrf_gpio_pin_read(button));
}
