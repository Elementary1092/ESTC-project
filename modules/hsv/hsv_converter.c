#include <nrfx.h>
#include <nrf_log.h>
#include <math.h>

#include "utils/numeric/ops.h"
#include "utils/numeric/converter.h"
#include "hsv_converter.h"

static void hsv_converter_align_hsv(hsv_ctx_t *hsv)
{
	if (hsv->hue > HSV_MAX_HUE || isnan(hsv->hue) || isinf(hsv->hue))
	{
		hsv->hue = HSV_MAX_HUE;
	}

	if (hsv->saturation > HSV_MAX_SATURATION || isnan(hsv->saturation) || isinf(hsv->saturation))
	{
		hsv->saturation = HSV_MAX_SATURATION;
	}

	if (hsv->brightness > HSV_MAX_BRIGHTNESS || isnan(hsv->brightness) || isinf(hsv->brightness))
	{
		hsv->brightness = HSV_MAX_BRIGHTNESS;
	}

	hsv->is_valid = true;
}

rgb_value_t hsv_converter_convert_hsv_to_rgb(float hue, float saturation, float brightness)
{
	rgb_value_t res;
	hsv_ctx_t hsv = (hsv_ctx_t){
		.hue = hue,
		.saturation = saturation,
		.brightness = brightness,
	};

	hsv_converter_align_hsv(&hsv);

	float M = 255.0F * brightness / 100.0F;
	float m = M * (1.0F - (saturation / 100.0F));
	float hue_mod_2 = utils_numeric_ops_modf((hue / 60.0F), 2);
	float z = (M - m) * (1 - utils_numeric_ops_absf(hue_mod_2 - 1));

	uint16_t shade0 = utils_numeric_converter_f_to_u16(M, HSV_CONVERTER_MAX_RGB);
	uint16_t shade1 = utils_numeric_converter_f_to_u16(m, HSV_CONVERTER_MAX_RGB);
	uint16_t shade2 = utils_numeric_converter_f_to_u16(z + m, HSV_CONVERTER_MAX_RGB);

	hue = hsv.hue;
	if (hue < 60.0F || hue == HSV_MAX_HUE)
	{
		res.red = shade0;
		res.green = shade2;
		res.blue = shade1;
	}
	else if (hue < 120.0F)
	{
		res.red = shade2;
		res.green = shade0;
		res.blue = shade1;
	}
	else if (hue < 180.0F)
	{
		res.red = shade1;
		res.green = shade0;
		res.blue = shade2;
	}
	else if (hue < 240.0F)
	{
		res.red = shade1;
		res.green = shade2;
		res.blue = shade0;
	}
	else if (hue < 300.0F)
	{
		res.red = shade2;
		res.green = shade1;
		res.blue = shade0;
	}
	else
	{
		res.red = shade0;
		res.green = shade1;
		res.blue = shade2;
	}

	return res;
}

// Used formula: https://mattlockyer.github.io/iat455/documents/rgb-hsv.pdf
hsv_ctx_t hsv_converter_convert_rgb_to_hsv(int16_t red, int16_t green, int16_t blue)
{
	hsv_ctx_t hsv;

	double R = ((double)red)/((double)HSV_CONVERTER_MAX_RGB);
	double G = ((double)green)/((double)HSV_CONVERTER_MAX_RGB);
	double B = ((double)blue)/((double)HSV_CONVERTER_MAX_RGB);

	double Cmax = MAX(MAX(R, G), B);
	double Cmin = MIN(MIN(R, G), B);

	double delta = Cmax - Cmin;

	hsv.brightness = Cmax * 100.0F;
	
	if (Cmax == 0)
	{
		hsv.saturation = 0.0F;
	}
	else
	{
		hsv.saturation = (float)(delta / Cmax * 100.0);
	}

	if (delta == 0)
	{
		hsv.hue = 0.0F;
	}
	else if (Cmax == R)
	{
		double int_part = 0.0;
		double fract_part = modf((G-B) / delta, &int_part);
		hsv.hue = (float)(60.0 * (fract_part + int_part));
	}
	else if (Cmax == G)
	{
		hsv.hue = (float)(60.0 * ((B - R) / delta + 2.0));
	}
	else
	{
		hsv.hue = (float)(60.0 * ((R - G) / delta + 4.0));
	}

	hsv_converter_align_hsv(&hsv);

	return hsv;
}
