#include <nrfx.h>
#include "hsv_helper.h"

#define HSV_MAX_HUE        360
#define HSV_MAX_SATURATION 100
#define HSV_MAX_BRIGHTNESS 100

void hsv_helper_convert(hsv_ctx_t *hsv, rgb_value_t *res)
{
	NRFX_ASSERT(hsv != NULL);
	NRFX_ASSERT(res != NULL);
	NRFX_ASSERT(hsv->hue <= HSV_MAX_HUE);
	NRFX_ASSERT(hsv->saturation <= HSV_MAX_SATURATION);
	NRFX_ASSERT(hsv->brightness <= HSV_MAX_BRIGHTNESS);

	uint16_t M = ( 255 * ( (uint16_t)hsv->brightness ) ) / 100;
	uint16_t m = M * ( 100 - ( (uint16_t)hsv->saturation ) ) / 100;
	uint16_t hue_mod_2 = ( hsv->hue / 60 ) % 2;
	uint16_t z = (M - m) * hue_mod_2;

	uint16_t hue = hsv->hue;
	if (hue < 60 || hue == 360)
	{
		res->red = M;
		res->green = z + m;
		res->blue = m;
	}
	else if (hue < 120)
	{
		res->red = z + m;
		res->green = M;
		res->blue = m;
	}
	else if (hue < 180)
	{
		res->red = m;
		res->green = M;
		res->blue = z + m;
	}
	else if (hue < 240)
	{
		res->red = m;
		res->green = z + m;
		res->blue = M;
	}
	else if (hue < 300)
	{
		res->red = z + m;
		res->green = m;
		res->blue = M;
	}
	else
	{
		res->red = M;
		res->green = m;
		res->blue = z + m;
	}
}