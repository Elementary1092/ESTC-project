#include <stdbool.h>
#include <hal/nrf_gpio.h>
#include "led.h"


void led_init_all(void)
{
	static bool is_inited = false;
	
	if (!is_inited)
	{
		/* Configuring these pins as output */
		nrf_gpio_cfg_output(LED_PCA10059_RED);
		nrf_gpio_cfg_output(LED_PCA10059_GREEN);
		nrf_gpio_cfg_output(LED_PCA10059_BLUE);
		nrf_gpio_cfg_output(LED_PCA10059_YELLOW);

		/* Switching off leds */
		nrf_gpio_pin_write(LED_PCA10059_RED, LED_SWITCH_OFF_SIGNAL);
		nrf_gpio_pin_write(LED_PCA10059_GREEN, LED_SWITCH_OFF_SIGNAL);
		nrf_gpio_pin_write(LED_PCA10059_BLUE, LED_SWITCH_OFF_SIGNAL);
		nrf_gpio_pin_write(LED_PCA10059_YELLOW, LED_SWITCH_OFF_SIGNAL);

		is_inited = true;
	}
}