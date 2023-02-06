#include "sinusoid.h"
#include <math.h>
#include "utils/numeric/ops.h"

#define UTILS_GENERATOR_SINUSOID_DEG_TO_RAD(x) 3.14F / 180.0F * ( x )

void utils_generator_sinusoid_next(utils_generator_sinusoid_ctx_t *generator_ctx)
{
	if (generator_ctx->current_angle > generator_ctx->max_angle_value)
	{
		generator_ctx->current_angle = 0.0F;
	}

	float radians = UTILS_GENERATOR_SINUSOID_DEG_TO_RAD(generator_ctx->angle_factor * generator_ctx->current_angle);
	generator_ctx->current_angle += generator_ctx->angle_step;
	generator_ctx->current_value = sinf(radians);
}