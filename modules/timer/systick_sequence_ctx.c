#include <unistd.h>
#include <nrfx_systick.h>
#include "systick_sequence_ctx.h"

#define MICROSECONDS_IN_SECOND           1000000
#define SYSTICK_SEQUENCE_CTX_INITIAL_IDX 5

void timer_systick_sequence_ctx_init(systick_sequence_ctx_t *ctx, uint32_t *on_seq_queue_us, uint16_t frequency)
{
	NRFX_ASSERT(ctx != NULL);
	NRFX_ASSERT(on_seq_queue_us != NULL);

	ctx->on_seq_us = on_seq_queue_us;
	ctx->should_inc = SYSTICK_SEQUENCE_CTX_INITIAL_IDX < SYSTICK_CTX_ON_SEQ_SIZE;
	ctx->idx = SYSTICK_SEQUENCE_CTX_INITIAL_IDX;

	uint32_t cycle_step_us = (MICROSECONDS_IN_SECOND / ((uint32_t)frequency)) / (SYSTICK_CTX_ON_SEQ_SIZE - 1);
	uint32_t on_delay_us = 0;
	for (ssize_t i = 0; i < SYSTICK_CTX_ON_SEQ_SIZE; i++)
	{
		ctx->on_seq_us[i] = on_delay_us;
		on_delay_us += cycle_step_us;
	}

	ctx->seq_cycle_us = ctx->on_seq_us[SYSTICK_CTX_ON_SEQ_SIZE - 1];

	nrfx_systick_init();
}

bool timer_systick_sequence_ctx_has_time_elapsed(systick_sequence_ctx_t *ctx, bool on_time)
{
	NRFX_ASSERT(ctx != NULL);

	if (ctx->idx >= SYSTICK_CTX_ON_SEQ_SIZE)
	{
		ctx->idx = SYSTICK_CTX_ON_SEQ_SIZE - 1;
		ctx->should_inc = false;
	}
	else if (ctx->idx == 0)
	{
		ctx->should_inc = true;
	}

	uint32_t delay = ctx->on_seq_us[ctx->idx];

	if (!on_time)
	{
		delay = ctx->seq_cycle_us - delay;
	}

	nrfx_systick_state_t state;
	nrfx_systick_get(&state);

	if (nrfx_systick_test(&state, delay)) 
	{
		if (!on_time)
		{
			ctx->idx = (ctx->should_inc) ? (ctx->idx + 1) : (ctx->idx - 1);
		}

		return true;
	}

	return false;
}