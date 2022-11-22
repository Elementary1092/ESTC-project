#include "board_button.h"
#include <hal/nrf_gpiote.h>
#include <app_timer.h>
#include <nrfx_atomic.h>
#include <nrf_log.h>

#define BUTTON_PRESSED_DELAY_TICKS       APP_TIMER_TICKS(20)
#define BUTTON_PRESSED_RESET_DELAY_TICKS APP_TIMER_TICKS(200)
#define BUTTON_HOLD_PROCESS_DELAY_TICKS  APP_TIMER_TICKS(30)

#define BUTTON_MAX_STATE_SUBSCRIBERS 5

static button_clicks_subscriber_t volatile clicks_subscribers[BUTTON_RECENT_STATES_NUMBER][BUTTON_MAX_STATE_SUBSCRIBERS];
static uint8_t volatile clicks_subscribers_idxs[BUTTON_RECENT_STATES_NUMBER] = {0};

static button_hold_subscriber_t volatile hold_subcribers[BUTTON_MAX_STATE_SUBSCRIBERS];
static uint8_t volatile hold_subcribers_idx = 0;

APP_TIMER_DEF(button_click_reset_timer);
APP_TIMER_DEF(button_click_register_timer);
APP_TIMER_DEF(button_hold_process_timer);

static nrfx_atomic_u32_t recent_button_event_cnt = 0UL;

static void SW1_IRQHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	NRF_LOG_INFO("board_button: Handling interrupt on %ld pin", pin);
	app_timer_stop(button_click_register_timer);
	app_timer_start(button_click_register_timer, BUTTON_PRESSED_DELAY_TICKS, (void*)pin);
}

static void button_process_clicks_subscribers(void *p_ctx)
{
	board_button_t pin = *((uint32_t *)p_ctx);
	button_recent_state_t state = button_get_recent_state(pin);
	if (state == BUTTON_UNKNOWN_RECENT_STATE)
	{
		return;
	}

	for (uint8_t i = 0; i < clicks_subscribers_idxs[state]; i++)
	{
		(*clicks_subscribers[state][i])();
	}
}

static void button_click_register_handler(void *p_context)
{
	NRF_LOG_INFO("board_button: Registering click");
	NRF_LOG_INFO("board_button: Recent clicks number: %ld", nrfx_atomic_u32_add(&recent_button_event_cnt, 1UL));
	button_process_clicks_subscribers(p_context);
	app_timer_stop(button_click_reset_timer);
	app_timer_start(button_click_reset_timer, BUTTON_PRESSED_RESET_DELAY_TICKS, p_context);
}

static void button_click_reset_handler(void *p_context)
{
	NRF_LOG_INFO("board_button: Resetting clicks counter");
	nrfx_atomic_u32_store(&recent_button_event_cnt, 0UL);
}

static void button_hold_process_handler(void *p_ctx)
{
	if (button_is_pressed(BOARD_BUTTON_SW1))
	{
		for (uint8_t i = 0; i < hold_subcribers_idx; i++)
		{
			(*hold_subcribers[i])();
		}
	}
}

void button_init(board_button_t button)
{
	nrfx_gpiote_in_config_t cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
	cfg.pull = NRF_GPIO_PIN_PULLUP;
	nrfx_gpiote_in_init(button, &cfg, SW1_IRQHandler);
	nrfx_gpiote_in_event_enable(button, true);

	app_timer_create(&button_click_register_timer, APP_TIMER_MODE_SINGLE_SHOT, button_click_register_handler);
	app_timer_create(&button_click_reset_timer, APP_TIMER_MODE_SINGLE_SHOT, button_click_reset_handler);
	app_timer_create(&button_hold_process_timer, APP_TIMER_MODE_REPEATED, button_hold_process_handler);

	app_timer_start(button_hold_process_timer, BUTTON_HOLD_PROCESS_DELAY_TICKS, NULL);
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

void button_subscribe_to_SW1_state(button_recent_state_t state, button_clicks_subscriber_t handler)
{
	NRFX_ASSERT(state < BUTTON_RECENT_STATES_NUMBER);
	NRFX_ASSERT(handler != NULL);

	if (state == BUTTON_UNKNOWN_RECENT_STATE)
	{
		NRF_LOG_INFO("board_button: Cannot subscribe for unknown state");
		return;
	}

	uint8_t sub_idx = clicks_subscribers_idxs[state];
	if (sub_idx >= BUTTON_MAX_STATE_SUBSCRIBERS - 1)
	{
		NRF_LOG_INFO("board_button: Buffer overflow. Cannot add subscriber to %d state.", state);
		return;
	}

	clicks_subscribers[state][sub_idx] = handler;
	clicks_subscribers_idxs[state]++;
}

void button_subscribe_to_SW1_hold(button_hold_subscriber_t handler)
{
	NRFX_ASSERT(handler != NULL);

	if (hold_subcribers_idx >= BUTTON_MAX_STATE_SUBSCRIBERS - 1)
	{
		NRF_LOG_INFO("board_button: Buffer overflow. Cannot add subscriber for a hold state.");
		return;
	}

	hold_subcribers[hold_subcribers_idx++] = handler;
}
