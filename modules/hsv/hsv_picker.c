#include <nrf_log.h>
#include <app_timer.h>
#include "../gpio/led/led.h"
#include "../gpio/button/board_button.h"
#include "../flash/flash_memory.h"
#include "../../utils/numeric/ops.h"
#include "hsv_picker.h"
#include "hsv_converter.h"
#include <math.h>

#define RGB_CTX_VALUE_FACTOR 100U
#define PWM_CFG_TOP_VALUE HSV_CONVERTER_MAX_RGB *RGB_CTX_VALUE_FACTOR

#define PWM_INDICATOR_HUE_MODE_FACTOR 2.0F
#define PWM_INDICATOR_SATURATION_MODE_FACTOR 8.0F
#define PWM_INDICATOR_FUNCTION_FACTOR 3.14F / 180.0F

#define HSV_PICKER_HUE_STEP 0.5F
#define HSV_PICKER_SATURATION_STEP 0.5F
#define HSV_PICKER_BRIGHTNESS_STEP 0.5F

#define HSV_SAVED_VALUE_CONTROL_WORD 0xF0F0F0F0
#define HSV_SAVED_VALUE_OFFSET 0UL

#define HSV_SAVED_BUFFER_IDX_HUE 0
#define HSV_SAVED_BUFFER_IDX_SATUR 1
#define HSV_SAVED_BUFFER_IDX_BRIGHT 2

typedef union
{
	float fl;
	uint32_t ui;
} float_uint32_union_t;

static bool should_inc_saturation = false;
static bool should_inc_brightness = false;

static bool should_update_saved_value = false;

static bool is_pwm_inited = false;

static float indicator_function_angle = 0.0F;
static float indicator_function_factor = 0.0F;

static float const pwm_cfg_top_value_f = (float)PWM_CFG_TOP_VALUE;

static nrfx_pwm_t pwm_rgb = NRFX_PWM_INSTANCE(0);

static nrf_pwm_values_individual_t leds_values =
	{
		.channel_0 = 0,
		.channel_1 = 0,
		.channel_2 = 0,
		.channel_3 = 0};

static nrf_pwm_sequence_t const leds_seq =
	{
		.values.p_individual = &leds_values,
		.length = NRF_PWM_VALUES_LENGTH(leds_values),
		.repeats = 0,
		.end_delay = 0};

// config of pwm instance which should control all leds
static nrfx_pwm_config_t const leds_pwm_cfg =
	{
		.output_pins =
			{
				LED_PCA10059_RED | NRFX_PWM_PIN_INVERTED,
				LED_PCA10059_GREEN | NRFX_PWM_PIN_INVERTED,
				LED_PCA10059_BLUE | NRFX_PWM_PIN_INVERTED,
				LED_PCA10059_YELLOW | NRFX_PWM_PIN_INVERTED},
		.irq_priority = APP_IRQ_PRIORITY_LOWEST,
		.base_clock = NRF_PWM_CLK_1MHz,
		.count_mode = NRF_PWM_MODE_UP,
		.top_value = PWM_CFG_TOP_VALUE,
		.load_mode = NRF_PWM_LOAD_INDIVIDUAL,
		.step_mode = NRF_PWM_STEP_AUTO};

// This variable stores current value of hue, saturation & value (brightness)
static hsv_ctx_t hsv_ctx;

// This variable stores current value of rgb (which is calculated using hsv_ctx & hsv_converter_convert_hsv_to_rgb)
static rgb_value_t rgb_ctx;

// Current mode of hsv picker is stored (HSV_PICKER_(VIEW_MODE, EDIT_HUE_MODE, EDIT_SATURATION_MODE, EDIT_BRIGHTNESS_MODE))
static hsv_picker_mode_t curr_mode;

// Updates value of indicator which shows if cyclic variable should be incremented or decremented
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

/*
	Adds to value or subscribes from value step &
	sets indicator which shows if cyclic variable should be incremented or decremented
*/
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

// Sets up fields of hsv_ctx variable
static void hsv_picker_init_hsv_ctx(float hue, float saturation, float brightness)
{
	hsv_ctx.hue = hue;
	hsv_ctx.saturation = saturation;
	hsv_ctx.brightness = brightness;
}

// Starts playback of pwm using default flags & sets up to play sequence 1 time
static void hsv_picker_flush_pwm_values(void)
{
	// Starting playback using default flags & sequence will be played one time
	nrfx_pwm_simple_playback(&pwm_rgb, &leds_seq, 1, 0);
}

static void hsv_picker_update_rgb_ctx(uint16_t red, uint16_t green, uint16_t blue)
{
	rgb_ctx.red = red;
	rgb_ctx.green = green;
	rgb_ctx.blue = blue;
}

// Sets up values of rgb channels
static void hsv_picker_update_rgb_channels(void)
{
	leds_values.channel_0 = RGB_CTX_VALUE_FACTOR * rgb_ctx.red;
	leds_values.channel_1 = RGB_CTX_VALUE_FACTOR * rgb_ctx.green;
	leds_values.channel_2 = RGB_CTX_VALUE_FACTOR * rgb_ctx.blue;
}

// Converts hsv to rgb & sets up values of channels which are used to display values of rgb leds
static void hsv_picker_update_rgb(void)
{
	hsv_converter_convert_hsv_to_rgb(&hsv_ctx, &rgb_ctx);

	hsv_picker_update_rgb_channels();
}

/*
	If hsv_picker is in VIEW or EDIT BRIGHTNESS mode returns.
	Otherwise, generates pwm value of indicator led using formula:

		PWM_CFG_TOP_VALUE * sin(PWM_INDICATOR_FUNCTION_VALUE * indicator_function_angle * indicator_function_factor),
			where
				  PWM_CFG_TOP_VALUE            = top value from the pwm config;
				  PWM_INDICATOR_FUNCTION_VALUE = PI / 180 (used to convert degrees to radians);
				  indicator_function_angle     = variable to determine in which phase of cycle is indicator;
				  indicator_function_factor    = used to shrink/expand sine function (makes indicator blink faster/slower);

	After generating new value:
	- increments indicator_function_angle_value;
	- loads channel of pwm instance used to store value of indicator with generated value.
*/
static void hsv_picker_generate_indicator_playback(void)
{
	if (curr_mode == HSV_PICKER_MODE_VIEW || curr_mode == HSV_PICKER_MODE_EDIT_BRIGHTNESS)
	{
		return;
	}

	float new_value_factor = sinf(PWM_INDICATOR_FUNCTION_FACTOR * indicator_function_angle * indicator_function_factor);
	float new_value_f = pwm_cfg_top_value_f * utils_numeric_ops_absf(new_value_factor);

	indicator_function_angle += 1.0F;
	if (indicator_function_angle > 360.0F)
	{
		indicator_function_angle = 0.0F;
	}

	leds_values.channel_3 = hsv_converter_float_to_uint16(new_value_f, PWM_CFG_TOP_VALUE);
}

/*
	If hsv_picker mode is:
	- HSV_PICKER_MODE_VIEW:            set value of pwm channel of indicator to 0;
	- HSV_PICKER_MODE_EDIT_HUE:        update indicator_function_factor & reset indicator_function_angle;
	- HSV_PICKER_MODE_EDIT_SATURATION: the same as HSV_PICKER_MODE_EDIT_HUE;
	- HSV_PICKER_MODE_EDIT_BRIGHTNESS: set value of pwm channel of indicator to PWM_CFG_TOP_VALUE;

	After this new values flushed to pwm
*/
static void hsv_picker_start_indicator_playback(void)
{
	NRF_LOG_INFO("hsv_picker: Changing indicator playback.");

	switch (curr_mode)
	{
	case HSV_PICKER_MODE_VIEW:
		leds_values.channel_3 = 0;
		break;

	case HSV_PICKER_MODE_EDIT_HUE:
		indicator_function_factor = PWM_INDICATOR_HUE_MODE_FACTOR;
		indicator_function_angle = 0.0F;
		break;

	case HSV_PICKER_MODE_EDIT_SATURATION:
		indicator_function_factor = PWM_INDICATOR_SATURATION_MODE_FACTOR;
		indicator_function_angle = 0.0F;
		break;

	case HSV_PICKER_MODE_EDIT_BRIGHTNESS:
		leds_values.channel_3 = PWM_CFG_TOP_VALUE;
		break;

	default:
		return;
	}

	hsv_picker_generate_indicator_playback();
	hsv_picker_flush_pwm_values();
}

/*
	If event_type = NRFX_PWM_EVT_FINISHED (playback is finished),
	generate new pwm value of indicator and flush this value.
*/
static void pwm_event_handler(nrfx_pwm_evt_type_t event)
{
	if (event != NRFX_PWM_EVT_FINISHED)
	{
		return;
	}

	hsv_picker_generate_indicator_playback();
	hsv_picker_flush_pwm_values();
}

static void hsv_picker_init_pwm_module(void)
{
	if (!is_pwm_inited)
	{
		nrfx_err_t err_code = nrfx_pwm_init(&pwm_rgb, &leds_pwm_cfg, pwm_event_handler);
		APP_ERROR_CHECK(err_code);
		is_pwm_inited = true;
	}
}

/*
	Tries to initialize hsv from the flash.
	Return true if function succeds.
	Otherwise, false is returned.
*/
static bool hsv_picker_try_init_from_flash(void)
{
	uint32_t buffer[3] = {0UL};
	flash_memory_err_t err = flash_memory_read(buffer, 3UL, HSV_SAVED_VALUE_OFFSET, HSV_SAVED_VALUE_CONTROL_WORD, FLASH_MEMORY_NO_FLAGS);
	if (err != FLASH_MEMORY_NO_ERR)
	{
		NRF_LOG_INFO("hsv_picker_try_init_from_flash: Could not initialize from the flash. Error: %d", err);
		return false;
	}

	float_uint32_union_t hue;
	hue.ui = buffer[HSV_SAVED_BUFFER_IDX_HUE];
	float_uint32_union_t satur;
	satur.ui = buffer[HSV_SAVED_BUFFER_IDX_SATUR];
	float_uint32_union_t bright;
	bright.ui = buffer[HSV_SAVED_BUFFER_IDX_BRIGHT];

	hsv_ctx.hue = hue.fl;
	hsv_ctx.saturation = satur.fl;
	hsv_ctx.brightness = bright.fl;

	NRF_LOG_INFO("HSV values from the flash: %x, %x, %x.", hue.fl, satur.fl, bright.fl);

	return true;
}

static void hsv_picker_update_saved_value(void)
{
	float_uint32_union_t hue;
	hue.fl = hsv_ctx.hue;
	float_uint32_union_t satur;
	satur.fl = hsv_ctx.saturation;
	float_uint32_union_t bright;
	bright.fl = hsv_ctx.brightness;

	uint32_t buffer[3] = {hue.ui, satur.ui, bright.ui};
	flash_memory_err_t err = flash_memory_write(buffer, 3UL, HSV_SAVED_VALUE_OFFSET, HSV_SAVED_VALUE_CONTROL_WORD, FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE);
	if (err != FLASH_MEMORY_NO_ERR)
	{
		NRF_LOG_INFO("hsv_picker_update_saved_value: Could not update saved value. Error: %d", err);
		return;
	}
}

void hsv_picker_init(float initial_hue, float initial_saturation, float initial_brightness)
{
	hsv_picker_init_pwm_module();

	if (!hsv_picker_try_init_from_flash())
	{
		hsv_picker_init_hsv_ctx(initial_hue, initial_saturation, initial_brightness);
	}

	hsv_picker_update_rgb();
	hsv_picker_flush_pwm_values();

	curr_mode = HSV_PICKER_MODE_VIEW;
}

void hsv_picker_next_mode(void)
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
		if (should_update_saved_value)
		{
			hsv_picker_update_saved_value();
			should_update_saved_value = false;
		}
		break;

	default:
		curr_mode = HSV_PICKER_MODE_VIEW;
		NRF_LOG_INFO("hsv_picker: Invalid picker mode encountered. Changed to view mode");
		break;
	}

	NRF_LOG_INFO("hsv_picker: Changed picker mode. New mode: %d", curr_mode);

	hsv_picker_start_indicator_playback();
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
		hsv_ctx.hue = utils_numeric_ops_modf(hsv_ctx.hue + HSV_PICKER_HUE_STEP, 360.0F);
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

	should_update_saved_value = true;
	hsv_picker_update_rgb();
	hsv_picker_flush_pwm_values();
}

void hsv_picker_set_hsv(float hue, float satur, float bright)
{
	hsv_picker_init_hsv_ctx(hue, satur, bright);
	hsv_picker_update_rgb();
	hsv_picker_update_saved_value();
	hsv_picker_flush_pwm_values();
}

void hsv_picker_set_rgb(uint32_t red, uint32_t green, uint32_t blue)
{
	hsv_picker_update_rgb_ctx((uint16_t)red, (uint16_t)green, (uint16_t)blue);
	hsv_converter_convert_rgb_to_hsv(&rgb_ctx, &hsv_ctx);
	hsv_picker_update_rgb();
	hsv_picker_update_saved_value();
	hsv_picker_flush_pwm_values();
}
