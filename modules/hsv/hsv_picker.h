#ifndef HSV_PICKER_H
#define HSV_PICKER_H

#include <nrfx_pwm.h>
#include <nrf_drv_pwm.h>
#include <app_util_platform.h>
#include "hsv_converter.h"

/**
 * @brief Modes of an hsv picker.
*/
typedef enum
{
	HSV_PICKER_MODE_VIEW,
	HSV_PICKER_MODE_EDIT_HUE,
	HSV_PICKER_MODE_EDIT_SATURATION,
	HSV_PICKER_MODE_EDIT_BRIGHTNESS
} hsv_picker_mode_t;

/**
 * @brief Initialize hsv_picker & start playing given hsv value according to given values.
*/
void hsv_picker_init(float initial_hue, float initial_saturation, float initial_brightness);

/**
 * @brief Change mode of hsv_picker.
 * 
 * @details
	1) HSV_PICKER_MODE_VIEW            -> 2
	2) HSV_PICKER_MODE_EDIT_HUE        -> 3
	3) HSV_PICKER_MODE_EDIT_SATURATION -> 4
	4) HSV_PICKER_MODE_EDIT_BRIGHTNESS -> 1
*/
void hsv_picker_next_mode(void);

/**
 * @brief Changes hue, saturation or brightness according to the current mode of hsv_picker.
*/
void hsv_picker_edit_param(void);

/**
 * @brief Displays given hsv using RGB LEDS.
*/
void hsv_picker_set_hsv(float hue, float satur, float bright);

/**
 * @brief Displays given rgb.
*/
void hsv_picker_set_rgb(uint32_t red, uint32_t green, uint32_t blue);

/**
 * @brief Fills hsv_ctx with hsv values of currently displayed color.
 * 
 * @attention If the passed hsv context is NULL returns immediatelly. 
 * 
 * @param[out] hsv_ctx A structure which will be filled by current hsv values.
*/
void hsv_picker_get_current_hsv(hsv_ctx_t *hsv_ctx);

/**
 * @brief Fills rgb_ctx with rgb values of currently displayed color.
 
 * @attention If the passed rgb context is NULL returns immediatelly. 
 * 
 * @param[out] rgb_ctx A structure which will be filled by current rgb values. 
*/
void hsv_picker_get_current_rgb(rgb_value_t *rgb_ctx);

#endif
