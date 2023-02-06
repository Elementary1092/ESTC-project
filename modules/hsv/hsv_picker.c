#include <nrf_log.h>
#include "modules/gpio/button/board_button.h"
#include "utils/numeric/ops.h"
#include "utils/numeric/converter.h"
#include "utils/generator/sinusoid.h"
#include "utils/generator/fcyclic_variable.h"
#include "hsv_picker.h"
#include "hsv_flash.h"
#include "hsv_pwm.h"
#include <math.h>

#define PWM_INDICATOR_HUE_MODE_FACTOR 2.0F
#define PWM_INDICATOR_SATURATION_MODE_FACTOR 8.0F

#define HSV_PICKER_HUE_STEP 0.5F
#define HSV_PICKER_SATURATION_STEP 0.5F
#define HSV_PICKER_BRIGHTNESS_STEP 0.5F

static bool should_update_saved_value = false;

static struct hsv_picker_color_ctx
{
	float hue;
	utils_generator_fcyclic_variable_ctx_t saturation;
	utils_generator_fcyclic_variable_ctx_t brightness;
} current_hsv_color_ctx;

// Current mode of hsv picker is stored (HSV_PICKER_(VIEW_MODE, EDIT_HUE_MODE, EDIT_SATURATION_MODE, EDIT_BRIGHTNESS_MODE))
static hsv_picker_mode_t curr_mode;

// Sets up fields of hsv_ctx variable
static void hsv_picker_init_hsv_ctx(float hue, float saturation, float brightness)
{
	current_hsv_color_ctx.hue = hue;
	current_hsv_color_ctx.saturation = utils_generator_fcyclic_variable_ctx_init(saturation, 0.0F, 100.0F, 0.5F);
	current_hsv_color_ctx.brightness = utils_generator_fcyclic_variable_ctx_init(brightness, 0.0F, 100.0F, 0.5F);

	NRFX_ASSERT(current_hsv_color_ctx.brightness.is_valid);
	NRFX_ASSERT(current_hsv_color_ctx.saturation.is_valid);
}

// Converts hsv to rgb & sets up values of channels which are used to display values of rgb leds
static void hsv_picker_convert_to_rgb_and_update(void)
{
	rgb_value_t rgb = hsv_converter_convert_hsv_to_rgb(current_hsv_color_ctx.hue,
													   current_hsv_color_ctx.saturation.current_value,
													   current_hsv_color_ctx.brightness.current_value);

	hsv_pwm_update_rgb_channels(rgb.red, rgb.green, rgb.blue);
	hsv_pwm_restart_playback();
}

void hsv_picker_init(float initial_hue, float initial_saturation, float initial_brightness)
{
	hsv_pwm_init();

	hsv_ctx_t hsv_from_flash = hsv_picker_flash_try_init();

	if (!hsv_from_flash.is_valid)
	{
		hsv_picker_init_hsv_ctx(initial_hue, initial_saturation, initial_brightness);
	}
	else
	{
		hsv_picker_init_hsv_ctx(hsv_from_flash.hue, hsv_from_flash.saturation, hsv_from_flash.brightness);
	}

	hsv_picker_convert_to_rgb_and_update();

	curr_mode = HSV_PICKER_MODE_VIEW;
}

void hsv_picker_next_mode(void)
{
	NRF_LOG_INFO("hsv_picker: Changing picker mode. Current mode: %d", curr_mode);

	switch (curr_mode)
	{
	case HSV_PICKER_MODE_VIEW:
		curr_mode = HSV_PICKER_MODE_EDIT_HUE;
		hsv_pwm_indicator_blink_smoothly(PWM_INDICATOR_HUE_MODE_FACTOR, 0.0F);
		break;

	case HSV_PICKER_MODE_EDIT_HUE:
		curr_mode = HSV_PICKER_MODE_EDIT_SATURATION;
		hsv_pwm_indicator_blink_smoothly(PWM_INDICATOR_SATURATION_MODE_FACTOR, 0.0F);
		break;

	case HSV_PICKER_MODE_EDIT_SATURATION:
		curr_mode = HSV_PICKER_MODE_EDIT_BRIGHTNESS;
		hsv_pwm_indicator_blink_constantly(UINT16_MAX);
		break;

	case HSV_PICKER_MODE_EDIT_BRIGHTNESS:
		curr_mode = HSV_PICKER_MODE_VIEW;
		if (should_update_saved_value)
		{
			hsv_ctx_t saving_hsv;
			hsv_picker_get_current_hsv(&saving_hsv);
			hsv_picker_flash_update_saved_value(&saving_hsv);
			should_update_saved_value = false;
		}
		hsv_pwm_indicator_blink_constantly(0U);
		break;

	default:
		curr_mode = HSV_PICKER_MODE_VIEW;
		NRF_LOG_INFO("hsv_picker: Invalid picker mode encountered. Changed to view mode");
		break;
	}

	NRF_LOG_INFO("hsv_picker: Changed picker mode. New mode: %d", curr_mode);
	hsv_pwm_restart_playback();
}

void hsv_picker_edit_param(void)
{
	switch (curr_mode)
	{
	case HSV_PICKER_MODE_VIEW:
		// No parameter should be edited in the view mode
		return;

	case HSV_PICKER_MODE_EDIT_HUE:
		// Maximum value of hue may be 360 and mod 360.1 will always generate values >= 360
		// and does not generate big inaccuracy
		current_hsv_color_ctx.hue = utils_numeric_ops_absf(
			utils_numeric_ops_modf(
				current_hsv_color_ctx.hue + HSV_PICKER_HUE_STEP,
				360.1F));
		break;

	case HSV_PICKER_MODE_EDIT_SATURATION:
		utils_generator_fcyclic_variable_next(&(current_hsv_color_ctx.saturation));
		break;

	case HSV_PICKER_MODE_EDIT_BRIGHTNESS:
		utils_generator_fcyclic_variable_next(&(current_hsv_color_ctx.brightness));
		break;

	default:
		NRF_LOG_ERROR("hsv_picker: Unknown edit state: %d", curr_mode);
		return;
	}

	should_update_saved_value = true;
	hsv_picker_convert_to_rgb_and_update();
}

void hsv_picker_set_hsv(float hue, float satur, float bright)
{	
	hsv_ctx_t saving_hsv;
	hsv_picker_get_current_hsv(&saving_hsv);
	hsv_picker_flash_update_saved_value(&saving_hsv);
	hsv_picker_init_hsv_ctx(hue, satur, bright);

	hsv_picker_convert_to_rgb_and_update();
}

void hsv_picker_set_rgb(uint32_t red, uint32_t green, uint32_t blue)
{
	hsv_ctx_t hsv_ctx = hsv_converter_convert_rgb_to_hsv(red, green, blue);
	hsv_picker_init_hsv_ctx(hsv_ctx.hue, hsv_ctx.saturation, hsv_ctx.brightness);
	hsv_picker_flash_update_saved_value(&hsv_ctx);

	hsv_pwm_update_rgb_channels(red, green, blue);
	hsv_pwm_restart_playback();
}

void hsv_picker_get_current_hsv(hsv_ctx_t *hsv)
{
	if (hsv == NULL)
	{
		return;
	}
	
	hsv->hue = current_hsv_color_ctx.hue;
	hsv->saturation = current_hsv_color_ctx.saturation.current_value;
	hsv->brightness = current_hsv_color_ctx.brightness.current_value;
}

void hsv_picker_get_current_rgb(rgb_value_t *rgb)
{
	if (rgb == NULL)
	{
		return;
	}

	rgb_value_t current_rgb = hsv_converter_convert_hsv_to_rgb(current_hsv_color_ctx.hue,
															   current_hsv_color_ctx.saturation.current_value,
															   current_hsv_color_ctx.brightness.current_value);

	rgb->red = current_rgb.red;
	rgb->green = current_rgb.green;
	rgb->blue = current_rgb.blue;
}
