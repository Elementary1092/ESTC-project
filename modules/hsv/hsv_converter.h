#ifndef HSV_CONVERTER_H
#define HSV_CONVERTER_H

#include <stdint.h>

#define HSV_MAX_HUE 360.0F
#define HSV_MAX_SATURATION 100.0F
#define HSV_MAX_BRIGHTNESS 100.0F

#define HSV_CONVERTER_MAX_RGB 255U

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

void hsv_converter_convert_hsv_to_rgb(hsv_ctx_t *hsv, rgb_value_t *rgb);

void hsv_converter_convert_rgb_to_hsv(rgb_value_t *rgb, hsv_ctx_t *hsv);

uint16_t hsv_converter_float_to_uint16(float value, uint16_t max_value);

#endif