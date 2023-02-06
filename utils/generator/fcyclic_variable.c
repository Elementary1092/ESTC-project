#include "fcyclic_variable.h"


utils_generator_fcyclic_variable_ctx_t utils_generator_fcyclic_variable_ctx_init(float starting_value,
	                                                                            float min_value,
																			    float max_value,
																			    float step)
{
	if (min_value > max_value)
	{
		float temp = min_value;
		min_value = max_value;
		max_value = temp;
	}

	if (step > max_value - min_value || step == 0.0F)
	{
		return (utils_generator_fcyclic_variable_ctx_t){
			.is_valid = false
		};
	}

	bool should_increase = true;
	if (starting_value > max_value)
	{
		starting_value = max_value;
		should_increase = false;
	}
	else if (starting_value < min_value)
	{
		starting_value = min_value;

	}

	return (utils_generator_fcyclic_variable_ctx_t){
		.current_value = starting_value,
		.is_valid = true,
		.max_value = max_value,
		.min_value = min_value,
		.is_increasing = should_increase,
		.step = step,
	};
}

void utils_generator_fcyclic_variable_next(utils_generator_fcyclic_variable_ctx_t * const generator_ctx)
{
	if (!(generator_ctx->is_valid))
	{
		return;
	}

	if (generator_ctx->is_increasing)
	{
		generator_ctx->current_value += generator_ctx->step;
	}
	else
	{
		generator_ctx->current_value -= generator_ctx->step;
	}

	if (generator_ctx->current_value > generator_ctx->max_value)
	{
		generator_ctx->current_value = generator_ctx->max_value;
		generator_ctx->is_increasing = false;
	}
	else if (generator_ctx->current_value < generator_ctx->min_value)
	{
		generator_ctx->current_value = generator_ctx->min_value;
		generator_ctx->is_increasing = true;
	}
}