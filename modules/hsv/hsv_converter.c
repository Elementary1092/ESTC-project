#include <nrfx.h>
#include <math.h>

#include "../../utils/numeric/ops.h"
#include "hsv_converter.h"

static float const max_uint16_f = (float)UINT16_MAX;

static void hsv_converter_align_hsv(hsv_ctx_t *hsv)
{
	if (hsv->hue > HSV_MAX_HUE)
	{
		hsv->hue = HSV_MAX_HUE;
	}

	if (hsv->saturation > HSV_MAX_SATURATION)
	{
		hsv->saturation = HSV_MAX_SATURATION;
	}

	if (hsv->brightness > HSV_MAX_BRIGHTNESS)
	{
		hsv->brightness = HSV_MAX_BRIGHTNESS;
	}
}

uint16_t hsv_converter_float_to_uint16(float value, uint16_t max_value)
{
	float max_value_f = (float)max_value;
	if (value > max_value_f || max_uint16_f < value)
	{
		return max_value;
	}
	if (value < 0.0F)
	{
		return 0U;
	}

	return (uint16_t)((unsigned int)(floorf(value)));
}

void hsv_converter_convert_hsv_to_rgb(hsv_ctx_t *hsv, rgb_value_t *res)
{
	NRFX_ASSERT(hsv != NULL);
	NRFX_ASSERT(res != NULL);

	hsv_converter_align_hsv(hsv);

	float M = 255.0F * hsv->brightness / 100.0F;
	float m = M * (1.0F - (hsv->saturation / 100.0F));
	float hue_mod_2 = utils_numeric_ops_modf((hsv->hue / 60.0F), 2);
	float z = (M - m) * (1 - utils_numeric_ops_absf(hue_mod_2 - 1));

	uint16_t shade0 = hsv_converter_float_to_uint16(M, HSV_CONVERTER_MAX_RGB);
	uint16_t shade1 = hsv_converter_float_to_uint16(m, HSV_CONVERTER_MAX_RGB);
	uint16_t shade2 = hsv_converter_float_to_uint16(z + m, HSV_CONVERTER_MAX_RGB);

	float hue = hsv->hue;
	if (hue < 60.0F || hue == HSV_MAX_HUE)
	{
		res->red = shade0;
		res->green = shade2;
		res->blue = shade1;
	}
	else if (hue < 120.0F)
	{
		res->red = shade2;
		res->green = shade0;
		res->blue = shade1;
	}
	else if (hue < 180.0F)
	{
		res->red = shade1;
		res->green = shade0;
		res->blue = shade2;
	}
	else if (hue < 240.0F)
	{
		res->red = shade1;
		res->green = shade2;
		res->blue = shade0;
	}
	else if (hue < 300.0F)
	{
		res->red = shade2;
		res->green = shade1;
		res->blue = shade0;
	}
	else
	{
		res->red = shade0;
		res->green = shade1;
		res->blue = shade2;
	}
}

void hsv_converter_convert_rgb_to_hsv(rgb_value_t *rgb, hsv_ctx_t *hsv)
{
	float R = (float)rgb->red;
	float G = (float)rgb->green;
	float B = (float)rgb->blue;

	float max = MAX(MAX(R, G), B);
	float min = MIN(MIN(R, G), B);

	hsv->brightness = max / 255.0F * 100.0F;
	if (max == 0)
	{
		hsv->saturation = 0;
	}
	else
	{
		hsv->saturation = (1.0F - min / max) * 100.0F;
	}

	float sqrt_acos_arg = R * (R - G) + G * (G - B) + B * (B - R);
	float acos_arg = (rgb->red - 0.5F * rgb->green - 0.5F * rgb->blue) * sqrtf(sqrt_acos_arg);
	float degree = acosf(acos_arg);

	if (rgb->blue > rgb->green)
	{
		hsv->hue = degree;
	}
	else
	{
		hsv->hue = 360.0F - degree;
	}
}
