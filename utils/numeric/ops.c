#include <math.h>
#include "ops.h"

float utils_numeric_ops_modf(float value, float mod_base)
{
	float res = value - (mod_base * floorf(value / mod_base));
	if (isnan(res))
	{
		return value;
	}

	return res;
}

float utils_numeric_ops_absf(float value)
{
	if (value < 0.0F)
	{
		return -value;
	}

	if (isnan(value))
	{
		return 0.0F;
	}

	return value;
}
