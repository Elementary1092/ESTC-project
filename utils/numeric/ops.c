#include <math.h>
#include "ops.h"

float utils_numeric_ops_modf(float value, float mod_base)
{
	float mod = fmodf(value, mod_base);
	if (isnan(mod) || isinf(mod))
	{
		return 0.0F;
	}
	
	return mod;
}

float utils_numeric_ops_absf(float value)
{
	if (isnan(value) || isinf(value))
	{
		return 0.0F;
	}
	
	return fabsf(value);
}
