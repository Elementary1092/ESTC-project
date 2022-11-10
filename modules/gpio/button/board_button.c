#include "board_button.h"
#include <hal/nrf_gpiote.h>
#include <app_timer.h>
#include <nrf_atomic.h>

#define BUTTON_PRESSED_DELAY_TICKS       APP_TIMER_TICKS(10)
#define BUTTON_PRESSED_RESET_DELAY_TICKS APP_TIMER_TICKS(5000)

APP_TIMER_DEF(button_pressed_reset_timer);
APP_TIMER_DEF(button_pressed_timer);

static uint32_t recent_pressed_cnt = 0UL;

static void SW1_IRQHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	app_timer_stop(button_pressed_timer);
	app_timer_start(button_pressed_timer, BUTTON_PRESSED_DELAY_TICKS, NULL);
}

static void button_pressed_timeout_handler(void *p_context)
{
	recent_pressed_cnt += 1UL;
	app_timer_stop(button_pressed_reset_timer);
	app_timer_start(button_pressed_reset_timer, BUTTON_PRESSED_RESET_DELAY_TICKS, NULL);
}

static void button_pressed_reset_timeout_handler(void *p_context)
{
	recent_pressed_cnt = 0UL;
}

void button_init(board_button_t button)
{
	nrfx_gpiote_in_config_t cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
	cfg.pull = NRF_GPIO_PIN_PULLUP;
	nrfx_gpiote_in_init(button, &cfg, SW1_IRQHandler);
	nrfx_gpiote_in_event_enable(button, true);

	app_timer_create(&button_pressed_timer, APP_TIMER_MODE_SINGLE_SHOT, button_pressed_timeout_handler);
	app_timer_create(&button_pressed_reset_timer, APP_TIMER_MODE_SINGLE_SHOT, button_pressed_reset_timeout_handler);
}

uint32_t button_is_pressed(board_button_t button)
{
	return !(nrf_gpio_pin_read(button));
}

button_recent_state_t button_get_recent_state(board_button_t button)
{
	switch (recent_pressed_cnt)
	{
		case 0:
			return BUTTON_NOT_PRESSED_RECENTLY;
		
		case 1:
			return BUTTON_PRESSED_ONCE_RECENTLY;
		
		case 2:
			return BUTTON_PRESSED_TWICE_RECENTLY;
	}

	return BUTTON_UNKNOWN_RECENT_STATE;
}
