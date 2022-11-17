#ifndef HSV_HELPER_H
#define HSV_HELPER_H

#include <stdint.h>

#define HSV_MAX_HUE        360.0F
#define HSV_MAX_SATURATION 100.0F
#define HSV_MAX_BRIGHTNESS 100.0F

#define HSV_HELPER_MAX_RGB 255U

typedef struct
{
	float hue;
	float saturation;
	float brightness;
} hsv_ctx_t;

typedef struct
{
	uint16_t red;
	uint16_t green;
	uint16_t blue;
} rgb_value_t;

void hsv_helper_convert(hsv_ctx_t *hsv, rgb_value_t *rgb);

float hsv_helper_modf(float value, float mod_base);

uint16_t hsv_helper_float_to_uint16(float value, uint16_t max_value);

float hsv_helper_absf(float value);

#endif