#include <math.h>
#include "converter.h"

static float const max_uint16_f = (float)UINT16_MAX;

uint16_t utils_numeric_converter_f_to_u16(float value, uint16_t max_value)
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
