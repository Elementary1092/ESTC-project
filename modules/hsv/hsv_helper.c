#include <nrfx.h>
#include <math.h>
#include "hsv_helper.h"

static float const max_uint16_f = (float)UINT16_MAX;

float hsv_helper_modf(float value, float mod_base)
{
	return value - (mod_base * floorf(value / mod_base));
}

static void hsv_helper_align_hsv(hsv_ctx_t *hsv)
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

uint16_t hsv_helper_float_to_uint16(float value, uint16_t max_value)
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

float hsv_helper_absf(float value)
{
	if (value < 0.0F)
	{
		return -value;
	}

	return value;
}

void hsv_helper_convert(hsv_ctx_t *hsv, rgb_value_t *res)
{
	NRFX_ASSERT(hsv != NULL);
	NRFX_ASSERT(res != NULL);

	hsv_helper_align_hsv(hsv);	

	float M = 255.0F * hsv->brightness / 100.0F;
	float m = M * ( 1.0F - ( hsv->saturation / 100.0F) );
	float hue_mod_2 = hsv_helper_modf( (hsv->hue / 60.0F ), 2);
	float z = (M - m) * ( 1 - hsv_helper_absf(hue_mod_2 - 1) );

	uint16_t shade0 = hsv_helper_float_to_uint16(M, HSV_HELPER_MAX_RGB);
	uint16_t shade1 = hsv_helper_float_to_uint16(m, HSV_HELPER_MAX_RGB);
	uint16_t shade2 = hsv_helper_float_to_uint16(z + m, HSV_HELPER_MAX_RGB); 

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