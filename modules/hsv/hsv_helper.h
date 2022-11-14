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
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} rgb_value_t;

void hsv_helper_convert(hsv_ctx_t *hsv, rgb_value_t *rgb);

#endif