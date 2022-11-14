#include <nrfx.h>
#include "hsv_helper.h"

#define HSV_MAX_HUE        360
#define HSV_MAX_SATURATION 100
#define HSV_MAX_BRIGHTNESS 100

static uint8_t uint16_to_uint8(uint16_t value)
{
	if (value > 255)
	{
		return 255;
	}

	return ( (uint8_t)value );
}

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

	uint8_t shade0 = uint16_to_uint8(M);
	uint8_t shade1 = uint16_to_uint8(z + m);
	uint8_t shade2 = uint16_to_uint8(m);

	uint16_t hue = hsv->hue;
	if (hue < 60 || hue == 360)
	{
		res->red = shade0;
		res->green = shade1;
		res->blue = shade2;
	}
	else if (hue < 120)
	{
		res->red = shade1;
		res->green = shade0;
		res->blue = shade2;
	}
	else if (hue < 180)
	{
		res->red = shade2;
		res->green = shade0;
		res->blue = shade1;
	}
	else if (hue < 240)
	{
		res->red = shade2;
		res->green = shade1;
		res->blue = shade0;
	}
	else if (hue < 300)
	{
		res->red = shade1;
		res->green = shade2;
		res->blue = shade0;
	}
	else
	{
		res->red = shade0;
		res->green = shade2;
		res->blue = shade1;
	}
}