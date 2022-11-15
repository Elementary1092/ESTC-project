#ifndef HSV_HELPER_H
#define HSV_HELPER_H

#include <stdint.h>

typedef struct
{
	uint16_t hue;
	uint8_t saturation;
	uint8_t brightness;
} hsv_ctx_t;

typedef struct
{
	uint16_t red;
	uint16_t green;
	uint16_t blue;
} rgb_value_t;

void hsv_helper_convert(hsv_ctx_t *hsv, rgb_value_t *rgb);

#endif