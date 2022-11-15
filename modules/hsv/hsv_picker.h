#ifndef HSV_PICKER_H
#define HSV_PICKER_H

#include <nrfx_pwm.h>
#include <nrf_drv_pwm.h>
#include <app_util_platform.h>

void hsv_picker_init(uint16_t initial_hue, uint8_t initial_saturation, uint8_t initial_brightness);

typedef enum
{
	HSV_PICKER_MODE_VIEW,
	HSV_PICKER_MODE_EDIT_HUE,
	HSV_PICKER_MODE_EDIT_SATURATION,
	HSV_PICKER_MODE_EDIT_BRIGHTNESS
} hsv_picker_mode_t;

void hsv_picker_next_mode(void);

void hsv_picker_edit_param(void);

#endif
