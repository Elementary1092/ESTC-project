#include "board_button.h"
#include <hal/nrf_gpiote.h>
#include <app_timer.h>
#include <nrfx_atomic.h>
#include <nrf_log.h>

#define BUTTON_PRESSED_DELAY_TICKS       APP_TIMER_TICKS(20)
#define BUTTON_PRESSED_RESET_DELAY_TICKS APP_TIMER_TICKS(200)

APP_TIMER_DEF(button_click_reset_timer);
APP_TIMER_DEF(button_click_register_timer);

static nrfx_atomic_u32_t recent_button_event_cnt = 0UL;

static void SW1_IRQHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	NRF_LOG_INFO("Handling interrupt on %ld pin", pin);
	app_timer_stop(button_click_register_timer);
	app_timer_start(button_click_register_timer, BUTTON_PRESSED_DELAY_TICKS, NULL);
}

static void button_click_register_handler(void *p_context)
{
	NRF_LOG_INFO("Registering click");
	NRF_LOG_INFO("Recent clicks number: %ld", nrfx_atomic_u32_add(&recent_button_event_cnt, 1UL));
	app_timer_stop(button_click_reset_timer);
	app_timer_start(button_click_reset_timer, BUTTON_PRESSED_RESET_DELAY_TICKS, p_context);
}

static void button_click_reset_handler(void *p_context)
{
	NRF_LOG_INFO("Resetting clicks counter");
	nrfx_atomic_u32_store(&recent_button_event_cnt, 0UL);
}

void button_init(board_button_t button)
{
	nrfx_gpiote_in_config_t cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
	cfg.pull = NRF_GPIO_PIN_PULLUP;
	nrfx_gpiote_in_init(button, &cfg, SW1_IRQHandler);
	nrfx_gpiote_in_event_enable(button, true);

	app_timer_create(&button_click_register_timer, APP_TIMER_MODE_SINGLE_SHOT, button_click_register_handler);
	app_timer_create(&button_click_reset_timer, APP_TIMER_MODE_SINGLE_SHOT, button_click_reset_handler);
}

uint32_t button_is_pressed(board_button_t button)
{
	return !(nrf_gpio_pin_read(button));
}

button_recent_state_t button_get_recent_state(board_button_t button)
{
	/*
		Current implementation increments clicks counter when button is pressed and released.
		So, to count number of clicks this counter's value should be divided by 2.
	*/

	switch (nrfx_atomic_u32_fetch_add(&recent_button_event_cnt, 0UL))
	{
		case 0:
			return BUTTON_NOT_PRESSED_RECENTLY;
		
		case 2:
			return BUTTON_PRESSED_ONCE_RECENTLY;
		
		case 4:
			return BUTTON_PRESSED_TWICE_RECENTLY;
	}

	return BUTTON_UNKNOWN_RECENT_STATE;
}
