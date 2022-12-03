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

/*
	Initialize hsv_picker & start playing given hsv value according to the given values
*/
void hsv_picker_init(float initial_hue, float initial_saturation, float initial_brightness);

/*
	Change mode of hsv_picker.
	1) HSV_PICKER_MODE_VIEW            -> 2
	2) HSV_PICKER_MODE_EDIT_HUE        -> 3
	3) HSV_PICKER_MODE_EDIT_SATURATION -> 4
	4) HSV_PICKER_MODE_EDIT_BRIGHTNESS -> 1
*/
void hsv_picker_next_mode(void);

/*
	Changes hue, saturation or birghtness according to the current mode of hsv_picker.
*/
void hsv_picker_edit_param(void);

void hsv_picker_set_hsv(float hue, float satur, float bright);

void hsv_picker_set_rgb(uint32_t red, uint32_t green, uint32_t blue);

#endif
