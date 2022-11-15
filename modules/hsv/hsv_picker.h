#ifndef HSV_PICKER_H
#define HSV_PICKER_H

#include <nrfx_pwm.h>
#include <nrf_drv_pwm.h>
#include <app_util_platform.h>

typedef enum
{
	HSV_PICKER_MODE_VIEW,
	HSV_PICKER_MODE_EDIT_HUE,
	HSV_PICKER_MODE_EDIT_SATURATION,
	HSV_PICKER_MODE_EDIT_BRIGHTNESS
} hsv_picker_mode_t;

void hsv_picker_init(float initial_hue, float initial_saturation, float initial_brightness);

void hsv_picker_next_mode(void);

void hsv_picker_edit_param(void);

#endif
