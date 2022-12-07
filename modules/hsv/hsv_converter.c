#include <nrfx.h>
#include <nrf_log.h>
#include <math.h>

#include "utils/numeric/ops.h"
#include "utils/numeric/converter.h"
#include "hsv_converter.h"

static void hsv_converter_align_hsv(hsv_ctx_t *hsv)
{
	if (hsv->hue > HSV_MAX_HUE || isnan(hsv->hue))
	{
		hsv->hue = HSV_MAX_HUE;
	}

	if (hsv->saturation > HSV_MAX_SATURATION || isnan(hsv->saturation))
	{
		hsv->saturation = HSV_MAX_SATURATION;
	}

	if (hsv->brightness > HSV_MAX_BRIGHTNESS || isnan(hsv->brightness))
	{
		hsv->brightness = HSV_MAX_BRIGHTNESS;
	}
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

	uint16_t shade0 = utils_numeric_converter_f_to_u16(M, HSV_CONVERTER_MAX_RGB);
	uint16_t shade1 = utils_numeric_converter_f_to_u16(m, HSV_CONVERTER_MAX_RGB);
	uint16_t shade2 = utils_numeric_converter_f_to_u16(z + m, HSV_CONVERTER_MAX_RGB);

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

// Used formula: https://mattlockyer.github.io/iat455/documents/rgb-hsv.pdf
void hsv_converter_convert_rgb_to_hsv(rgb_value_t *rgb, hsv_ctx_t *hsv)
{
	float R = ((float)rgb->red)/((float)HSV_CONVERTER_MAX_RGB);
	float G = ((float)rgb->green)/((float)HSV_CONVERTER_MAX_RGB);
	float B = ((float)rgb->blue)/((float)HSV_CONVERTER_MAX_RGB);

	float Cmax = MAX(MAX(R, G), B);
	float Cmin = MIN(MIN(R, G), B);

	float delta = Cmax - Cmin;

	hsv->brightness = Cmax * 100.0F;
	
	if (Cmax == 0)
	{
		hsv->saturation = 0;
	}
	else
	{
		hsv->saturation = delta / Cmax * 100.0F;
	}

	if (delta == 0)
	{
		hsv->hue = 0.0F;
	}
	else if (Cmax == R)
	{
		hsv->hue = 60.0F * utils_numeric_ops_modf((G - B) / delta, 6.0F);
	}
	else if (Cmax == G)
	{
		hsv->hue = 60.0F * ((B - R) / delta + 2.0F);
	}
	else
	{
		hsv->hue = 60.0F * ((R - G) / delta + 4.0F);
	}

	hsv_converter_align_hsv(hsv);
}
