#include <stdbool.h>
#include <stdint.h>

#include "modules/pwm/pwm_individual.h"
#include "modules/gpio/led/led.h"
#include "utils/generator/sinusoid.h"
#include "utils/numeric/converter.h"
#include "hsv_converter.h"
#include "hsv_pwm.h"

#define RGB_CTX_VALUE_FACTOR 100U
#define HSV_PWM_CFG_TOP_VALUE HSV_CONVERTER_MAX_RGB *RGB_CTX_VALUE_FACTOR

#define HSV_PWM_RED PWM_INDIVIDUAL_OUTPUT_PIN0
#define HSV_PWM_GREEN PWM_INDIVIDUAL_OUTPUT_PIN1
#define HSV_PWM_BLUE PWM_INDIVIDUAL_OUTPUT_PIN2
#define HSV_PWM_INDICATOR PWM_INDIVIDUAL_OUTPUT_PIN3

#define HSV_PWM_OUTPUT_PIN_RED LED_PCA10059_RED | NRFX_PWM_PIN_INVERTED
#define HSV_PWM_OUTPUT_PIN_GREEN LED_PCA10059_GREEN | NRFX_PWM_PIN_INVERTED
#define HSV_PWM_OUTPUT_PIN_BLUE LED_PCA10059_BLUE | NRFX_PWM_PIN_INVERTED
#define HSV_PWM_OUTPUT_PIN_INDICATOR LED_PCA10059_YELLOW | NRFX_PWM_PIN_INVERTED

static enum hsv_pwm_indicator_mode_e {
	HSV_PWM_INDICATOR_MODE_CONSTANT,
	HSV_PWM_INDICATOR_MODE_SINUSIOD,
} hsv_pwm_indicator_current_mode;

static utils_generator_sinusoid_ctx_t indicator_value_gen_ctx = {
	.angle_factor = 0.0F,
	.angle_step = 1.0F,
	.current_angle = 0.0F,
	.max_angle_value = 360.0F,
};

static float const pwm_cfg_top_value_f = (float)HSV_PWM_CFG_TOP_VALUE;

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
	if (hsv_pwm_indicator_current_mode == HSV_PWM_INDICATOR_MODE_CONSTANT)
	{
		return;
	}

	utils_generator_sinusoid_next(&indicator_value_gen_ctx);
	float new_value = pwm_cfg_top_value_f * indicator_value_gen_ctx.current_value;

	uint16_t new_indicator_value = utils_numeric_converter_f_to_u16(new_value, HSV_PWM_CFG_TOP_VALUE);

	pwm_individual_set_output_pin_value(HSV_PWM_INDICATOR, new_indicator_value);
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
	hsv_pwm_restart_playback();
}

void hsv_pwm_init(void)
{
	static bool is_pwm_inited;

	if (is_pwm_inited)
	{
		return;
	}

	pwm_individual_register_output_pin(HSV_PWM_RED, HSV_PWM_OUTPUT_PIN_RED);
	pwm_individual_register_output_pin(HSV_PWM_GREEN, HSV_PWM_OUTPUT_PIN_GREEN);
	pwm_individual_register_output_pin(HSV_PWM_BLUE, HSV_PWM_OUTPUT_PIN_BLUE);
	pwm_individual_register_output_pin(HSV_PWM_INDICATOR, HSV_PWM_OUTPUT_PIN_INDICATOR);

	pwm_individual_config_t config = (pwm_individual_config_t){
		.clock_base = NRF_PWM_CLK_1MHz,
		.count_mode = NRF_PWM_MODE_UP,
		.top_value = HSV_PWM_CFG_TOP_VALUE,
		.sequence_repeats = 0U,
		.sequence_end_delay = 0U,
	};

	pwm_individual_init(&config, pwm_event_handler);
	hsv_pwm_indicator_current_mode = HSV_PWM_INDICATOR_MODE_CONSTANT;
	is_pwm_inited = true;
}

// Sets up values of rgb channels
void hsv_pwm_update_rgb_channels(uint16_t red, uint16_t green, uint16_t blue)
{
	pwm_individual_set_output_pin_value(HSV_PWM_RED, RGB_CTX_VALUE_FACTOR * red);
	pwm_individual_set_output_pin_value(HSV_PWM_GREEN, RGB_CTX_VALUE_FACTOR * green);
	pwm_individual_set_output_pin_value(HSV_PWM_BLUE, RGB_CTX_VALUE_FACTOR * blue);
}

// Starts playback of pwm using default flags & sets up to play sequence 1 time
void hsv_pwm_restart_playback(void)
{
	pwm_individual_start_playback(1);
}

void hsv_pwm_indicator_blink_constantly(uint16_t value)
{
	if (value == UINT16_MAX)
	{
		value = HSV_PWM_CFG_TOP_VALUE;
	}

	pwm_individual_set_output_pin_value(HSV_PWM_INDICATOR, value);
	hsv_pwm_indicator_current_mode = HSV_PWM_INDICATOR_MODE_CONSTANT;
}

void hsv_pwm_indicator_blink_smoothly(float blinking_factor, float starting_value)
{
	indicator_value_gen_ctx.angle_factor = blinking_factor;
	indicator_value_gen_ctx.current_angle = 0.0F;
	hsv_pwm_indicator_current_mode = HSV_PWM_INDICATOR_MODE_SINUSIOD;
}
