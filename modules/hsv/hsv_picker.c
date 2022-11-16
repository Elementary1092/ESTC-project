#include <nrf_log.h>
#include <app_timer.h>
#include "../gpio/led/led.h"
#include "../gpio/button/board_button.h"
#include "hsv_picker.h"
#include "hsv_helper.h"

#define PWM_CFG_RGB_TOP_VALUE       255
#define PWM_CFG_INDICATOR_TOP_VALUE 10000
#define PWM_CFG_INDICATOR_STEP      200
#define INDICATOR_ARR_SIZE          (PWM_CFG_INDICATOR_TOP_VALUE / PWM_CFG_INDICATOR_STEP + 1) * 2

#define CHANGE_MODE_DELAY_TICKS APP_TIMER_TICKS(200)

#define HSV_PICKER_HUE_STEP        0.5F
#define HSV_PICKER_SATURATION_STEP 0.5F
#define HSV_PICKER_BRIGHTNESS_STEP 0.5F

APP_TIMER_DEF(change_mode_timer);

static bool should_inc_saturation = false;
static bool should_inc_brightness = false;

static nrfx_pwm_t pwm_rgb = NRFX_PWM_INSTANCE(0);
static nrfx_pwm_t pwm_edit_indicator = NRFX_PWM_INSTANCE(1);

static nrf_pwm_values_individual_t rgb_values = 
{
	.channel_0 = 0, 
	.channel_1 = 0, 
	.channel_2 = 0, 
	.channel_3 = 0
};

static nrf_pwm_values_common_t edit_indicator_high[1] = {PWM_CFG_INDICATOR_TOP_VALUE};

static bool is_indicator_seq_inited = false;
static nrf_pwm_values_common_t edit_indicator_values[INDICATOR_ARR_SIZE];

static nrf_pwm_sequence_t const rgb_seq =
{
	.values.p_individual = &rgb_values,
	.length = NRF_PWM_VALUES_LENGTH(rgb_values),
	.repeats = 0,
	.end_delay = 0
};

// config of rgb leds
static nrfx_pwm_config_t const rgb_cfg = 
{
	.output_pins = 
	{
		LED_PCA10059_RED | NRFX_PWM_PIN_INVERTED,
		LED_PCA10059_GREEN | NRFX_PWM_PIN_INVERTED,
		LED_PCA10059_BLUE | NRFX_PWM_PIN_INVERTED,
		NRFX_PWM_PIN_NOT_USED
	},
	.irq_priority = APP_IRQ_PRIORITY_LOWEST,
	.base_clock = NRF_PWM_CLK_125kHz,
	.count_mode = NRF_PWM_MODE_UP,
	.top_value = PWM_CFG_RGB_TOP_VALUE,
	.load_mode = NRF_PWM_LOAD_INDIVIDUAL,
	.step_mode = NRF_PWM_STEP_AUTO
};

static nrf_pwm_sequence_t const edit_indicator_high_seq =
{
	.values = edit_indicator_high,
	.length = NRF_PWM_VALUES_LENGTH(edit_indicator_high),
	.repeats = 0,
	.end_delay = 0
};

static nrf_pwm_sequence_t const edit_indicator_seq =
{
	.values = edit_indicator_values,
	.length = NRF_PWM_VALUES_LENGTH(edit_indicator_values),
	.repeats = 0,
	.end_delay = 0
};

// config of indicator led when hue is edited & when hsv is not edited
static nrfx_pwm_config_t const cfg_edit_hue_ind =
{
	.output_pins = 
	{
		LED_PCA10059_YELLOW | NRFX_PWM_PIN_INVERTED,
		NRFX_PWM_PIN_NOT_USED,
		NRFX_PWM_PIN_NOT_USED,
		NRFX_PWM_PIN_NOT_USED
	},
	.irq_priority = APP_IRQ_PRIORITY_LOWEST,
	.base_clock = NRF_PWM_CLK_1MHz,
	.count_mode = NRF_PWM_MODE_UP_AND_DOWN,
	.top_value = PWM_CFG_INDICATOR_TOP_VALUE,
	.load_mode = NRF_PWM_LOAD_COMMON,
	.step_mode = NRF_PWM_STEP_AUTO,
};

// config of indicator led when saturation & brightness is edited
static nrfx_pwm_config_t const cfg_edit_sat_ind =
{
	.output_pins = 
	{
		LED_PCA10059_YELLOW,
		NRFX_PWM_PIN_NOT_USED,
		NRFX_PWM_PIN_NOT_USED,
		NRFX_PWM_PIN_NOT_USED
	},
	.irq_priority = APP_IRQ_PRIORITY_LOWEST,
	.base_clock = NRF_PWM_CLK_4MHz,
	.count_mode = NRF_PWM_MODE_UP_AND_DOWN,
	.top_value = PWM_CFG_INDICATOR_TOP_VALUE,
	.load_mode = NRF_PWM_LOAD_COMMON,
	.step_mode = NRF_PWM_STEP_AUTO,
};

static hsv_ctx_t hsv_ctx;

static rgb_value_t rgb_ctx;

static hsv_picker_mode_t curr_mode;

static void hsv_picker_init_indicator_seq(void)
{
	uint16_t delay = 0;
	size_t border = INDICATOR_ARR_SIZE / 2;
	for (size_t i = 0; i < border; i++)
	{
		edit_indicator_values[i] = delay;
		edit_indicator_values[INDICATOR_ARR_SIZE - i - 1] = delay;
		delay += PWM_CFG_INDICATOR_STEP;
	}

	is_indicator_seq_inited = true;
}

static void hsv_picker_cyclic_var_set_indicator(float *value, float max_value, bool *indicator)
{
	if ((*value) <= 0.0F)
	{
		*indicator = true;
	}
	else if ((*value) >= max_value)
	{
		*indicator = false;
	}
}

static void hsv_picker_cyclic_var_next_value(float *value, float step, float max_value, bool *indicator)
{
	if (*indicator)
	{
		*value += step;
	}
	else
	{
		*value -= step;
	}

	hsv_picker_cyclic_var_set_indicator(value, max_value, indicator);
}

static void hsv_picker_init_hsv_ctx(float hue, float saturation, float brightness)
{
	hsv_ctx.hue = hue;
	hsv_ctx.saturation = saturation;
	hsv_ctx.brightness = brightness;
}

static void hsv_picker_display_rgb(void)
{
	nrfx_pwm_simple_playback(&pwm_rgb, &rgb_seq, 1, NRFX_PWM_FLAG_LOOP);
}

static void hsv_picker_update_rgb(void)
{
	hsv_helper_convert(&hsv_ctx, &rgb_ctx);

	rgb_values.channel_0 = rgb_ctx.red;
	rgb_values.channel_1 = rgb_ctx.green;
	rgb_values.channel_2 = rgb_ctx.blue;
}

static void hsv_picker_start_indicator_playback(void)
{
	nrfx_pwm_stop(&pwm_edit_indicator, false);

	NRF_LOG_INFO("hsv_picker: Changing indicator playback.");

	nrfx_err_t err_code;
	switch (curr_mode)
	{
	case HSV_PICKER_MODE_VIEW:
		// If hsv picker is in view mode, pwm_edit_indicator should use config with frequency 125Khz.
		// But starting playback is not necessary.
		nrfx_pwm_uninit(&pwm_edit_indicator);
		err_code = nrfx_pwm_init(&pwm_edit_indicator, &cfg_edit_hue_ind, NULL);
		APP_ERROR_CHECK(err_code);
		break;
	
	case HSV_PICKER_MODE_EDIT_HUE:
		err_code = nrfx_pwm_simple_playback(&pwm_edit_indicator, &edit_indicator_seq, 1, NRFX_PWM_FLAG_LOOP);
		APP_ERROR_CHECK(err_code);
		break;

	case HSV_PICKER_MODE_EDIT_SATURATION:
		// When hsv picker moves to edit saturation mode indicator should start blinking faster.
		// To make indicator to start blink faster frequency of this led should be changed.
		// So, pwm_edit_indicator config should be changed to update frequency.
		// To update frequency pwm_edit_indicator should be reinitialized.
		nrfx_pwm_uninit(&pwm_edit_indicator);
		err_code = nrfx_pwm_init(&pwm_edit_indicator, &cfg_edit_sat_ind, NULL);
		APP_ERROR_CHECK(err_code);

		err_code = nrfx_pwm_simple_playback(&pwm_edit_indicator, &edit_indicator_seq, 1, NRFX_PWM_FLAG_LOOP);
		APP_ERROR_CHECK(err_code);
		break;

	case HSV_PICKER_MODE_EDIT_BRIGHTNESS:
		err_code = nrfx_pwm_simple_playback(&pwm_edit_indicator, &edit_indicator_high_seq, 1, NRFX_PWM_FLAG_LOOP);
		APP_ERROR_CHECK(err_code);
		break;
	
	default:
		return;
	}
}

static void change_mode_handler(void *p_ctx)
{
	NRF_LOG_INFO("hsv_picker: Changing picker mode. Current mode: %d", curr_mode);

	switch (curr_mode)
	{
	case HSV_PICKER_MODE_VIEW:
		curr_mode = HSV_PICKER_MODE_EDIT_HUE;
		break;
	
	case HSV_PICKER_MODE_EDIT_HUE:
		curr_mode = HSV_PICKER_MODE_EDIT_SATURATION;
		break;

	case HSV_PICKER_MODE_EDIT_SATURATION:
		curr_mode = HSV_PICKER_MODE_EDIT_BRIGHTNESS;
		break;

	case HSV_PICKER_MODE_EDIT_BRIGHTNESS:
		curr_mode = HSV_PICKER_MODE_VIEW;
		break;
	
	default:
		curr_mode = HSV_PICKER_MODE_VIEW;
		NRF_LOG_INFO("hsv_picker: Invalid picker mode encountered. Changed to view mode");
		break;
	}

	NRF_LOG_INFO("hsv_picker: Changed picker mode. New mode: %d", curr_mode);
	
	hsv_picker_start_indicator_playback();
}

void hsv_picker_init(float initial_hue, float initial_saturation, float initial_brightness)
{
	if (!is_indicator_seq_inited)
	{
		hsv_picker_init_indicator_seq();
	}

	nrfx_err_t err_code = nrfx_pwm_init(&pwm_rgb, &rgb_cfg, NULL);
	APP_ERROR_CHECK(err_code);

	err_code = nrfx_pwm_init(&pwm_edit_indicator, &cfg_edit_hue_ind, NULL);
	APP_ERROR_CHECK(err_code);

	hsv_picker_init_hsv_ctx(initial_hue, initial_saturation, initial_brightness);

	hsv_picker_update_rgb();
	hsv_picker_display_rgb();

	curr_mode = HSV_PICKER_MODE_VIEW;

	app_timer_create(&change_mode_timer, APP_TIMER_MODE_SINGLE_SHOT, change_mode_handler);
}

void hsv_picker_next_mode(void)
{
	app_timer_stop(change_mode_timer);
	app_timer_start(change_mode_timer, CHANGE_MODE_DELAY_TICKS, NULL);
}

void hsv_picker_edit_param(void)
{
	float temp_value = 0.0F;
	switch (curr_mode)
	{
	case HSV_PICKER_MODE_VIEW:
		// No parameter should be edited in the view mode
		return;
	
	case HSV_PICKER_MODE_EDIT_HUE:
		// Maximum value of hue may be 360 and mod 360.1 will always generate values >= 360
		// and does not generate big inaccuracy
		hsv_ctx.hue = hsv_helper_modf(hsv_ctx.hue + HSV_PICKER_HUE_STEP, 360.1F);
		break;

	case HSV_PICKER_MODE_EDIT_SATURATION:
		temp_value = hsv_ctx.saturation;
		hsv_picker_cyclic_var_next_value(&temp_value, HSV_PICKER_SATURATION_STEP, HSV_MAX_SATURATION, &should_inc_saturation);
		hsv_ctx.saturation = temp_value;
		break;

	case HSV_PICKER_MODE_EDIT_BRIGHTNESS:
		temp_value = hsv_ctx.brightness;
		hsv_picker_cyclic_var_next_value(&temp_value, HSV_PICKER_BRIGHTNESS_STEP, HSV_MAX_BRIGHTNESS, &should_inc_brightness);
		hsv_ctx.brightness = temp_value;
		break;

	default:
		NRF_LOG_ERROR("hsv_picker: Unknown edit state: %d", curr_mode);
		return;
	}

	hsv_picker_update_rgb();
	hsv_picker_display_rgb();
}