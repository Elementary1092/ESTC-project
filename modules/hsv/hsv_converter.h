#ifndef HSV_CONVERTER_H
#define HSV_CONVERTER_H

#include <stdint.h>
#include <stdbool.h>

#define HSV_MAX_HUE 360.0F
#define HSV_MAX_SATURATION 100.0F
#define HSV_MAX_BRIGHTNESS 100.0F

#define HSV_CONVERTER_MAX_RGB 255U

/**
 * @brief A structure describing hsv color.
*/
typedef struct
{
	float hue;        /**< Hue value (may be in a range 0 - 360). */
	float saturation; /**< Saturation value (may be in a range 0 - 100). */
	float brightness; /**< Brightness value (may be in a range 0 - 100). */
	bool  is_valid;   /**< internal; Field guaranting that the instance of an object is valid. Do not change elsewhere. */
} hsv_ctx_t;

/**
 * @brief A structure decribing an rgb color.
*/
typedef struct
{
	uint16_t red;   /**< Red value (may be in a range from 0 - 255). */
	uint16_t green; /**< Green value (may be in a range from 0 - 255). */
	uint16_t blue;  /**< Blue value (may be in a range from 0 - 255). */
} rgb_value_t;

/**
 * @brief Converts from hsv to rgb.
 * 
 * @details Invalid hsv values will be aligned such that hsv_ctx_t will be a valid one.
*/
rgb_value_t hsv_converter_convert_hsv_to_rgb(float hue, float saturation, float brightness);

/**
 * @brief Converts from rgb to hsv.
*/
hsv_ctx_t hsv_converter_convert_rgb_to_hsv(int16_t red, int16_t green, int16_t blue);

#endif