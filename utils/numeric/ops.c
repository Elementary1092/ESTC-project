#include <math.h>
#include "ops.h"

float utils_numeric_ops_modf(float value, float mod_base)
{
	return value - (mod_base * floorf(value / mod_base));
}

float utils_numeric_ops_absf(float value)
{
	if (value < 0.0F)
	{
		return -value;
	}

	return value;
}
