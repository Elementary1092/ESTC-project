#include <stdbool.h>
#include <string.h>
#include "pwm_individual.h"

static bool is_inited = false;

static const nrfx_pwm_t global_pwm = NRFX_PWM_INSTANCE(0);

static nrf_pwm_values_individual_t pwm_values = {
	.channel_0 = 0U,
	.channel_1 = 0U,
	.channel_2 = 0U,
	.channel_3 = 0U,
};

static nrf_pwm_sequence_t pwm_sequence;

static uint8_t output_pins[4] = {0};

static pwm_individual_event_handler_t pwm_individual_event_handler;

static nrfx_pwm_config_t pwm_individual_config_init(pwm_individual_config_t *config)
{
	return (nrfx_pwm_config_t){
		.irq_priority = APP_IRQ_PRIORITY_LOWEST,
		.output_pins = {
			output_pins[0],
			output_pins[1],
			output_pins[2],
			output_pins[3],
		},
		.top_value = config->top_value,
		.step_mode = NRF_PWM_STEP_AUTO,
		.load_mode = NRF_PWM_LOAD_INDIVIDUAL,
		.count_mode = config->count_mode,
		.base_clock = config->clock_base,
	};
}

static nrf_pwm_sequence_t pwm_individual_sequence_config_init(pwm_individual_config_t *config)
{
	return (nrf_pwm_sequence_t){
		.values = {
			.p_individual = &pwm_values,
		},
		.length = NRF_PWM_VALUES_LENGTH(pwm_values),
		.repeats = config->sequence_repeats,
		.end_delay = config->sequence_end_delay,
	};
}

static void pwm_individual_register_event_handler(pwm_individual_event_handler_t event_handler)
{
	pwm_individual_event_handler = event_handler;
}

static void pwm_individual_run_event_handler(nrfx_pwm_evt_type_t event)
{
	if (pwm_individual_event_handler == NULL || !is_inited)
	{
		return;
	}

	pwm_individual_event_handler(event);

	nrfx_pwm_simple_playback(&global_pwm, &pwm_sequence, 1U, 0U);
}

void pwm_individual_register_output_pin(pwm_individual_output_pin_order_t pin_order, uint8_t pin_number)
{
	if (pin_order >= (sizeof(output_pins) / sizeof(output_pins[0])))
	{
		return;
	}

	output_pins[pin_order] = pin_number;
}

void pwm_individual_init(pwm_individual_config_t *config, pwm_individual_event_handler_t event_handler)
{
	if (is_inited)
	{
		return;
	}

	pwm_sequence = pwm_individual_sequence_config_init(config);
	nrfx_pwm_config_t pwm_config = pwm_individual_config_init(config);

	if (event_handler != NULL)
	{
		pwm_individual_register_event_handler(event_handler);
	}

	nrfx_err_t err_code = nrfx_pwm_init(&global_pwm, &pwm_config, pwm_individual_run_event_handler);
	APP_ERROR_CHECK(err_code);
	is_inited = true;
}

void pwm_individual_stop(void)
{
	nrfx_pwm_stop(&global_pwm, false);
	pwm_individual_event_handler = NULL;
	is_inited = false;
	memset(output_pins, 0, sizeof(output_pins) / sizeof(output_pins[0]));
}

void pwm_individual_set_output_pin_value(pwm_individual_output_pin_order_t pin_order, uint16_t new_value)
{
	switch (pin_order)
	{
	case PWM_INDIVIDUAL_OUTPUT_PIN0:
		pwm_values.channel_0 = new_value;
		break;
	case PWM_INDIVIDUAL_OUTPUT_PIN1:
		pwm_values.channel_1 = new_value;
		break;
	case PWM_INDIVIDUAL_OUTPUT_PIN2:
		pwm_values.channel_2 = new_value;
		break;
	case PWM_INDIVIDUAL_OUTPUT_PIN3:
		pwm_values.channel_3 = new_value;
		break;
	default:
		break;
	}
}

void pwm_individual_start_playback(uint16_t repeat_times)
{
	nrfx_pwm_simple_playback(&global_pwm, &pwm_sequence, repeat_times, 0U);
}
