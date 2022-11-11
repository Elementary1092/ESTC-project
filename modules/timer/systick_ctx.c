#include "systick_ctx.h"

static nrfx_systick_state_t state;

void timer_systick_ctx_init(void)
{	
	nrfx_systick_init();
}

void timer_systick_ctx_probe(void)
{
	nrfx_systick_get(&state);
}

bool timer_systick_ctx_has_time_elapsed(uint32_t delay)
{
	return nrfx_systick_test(&state, delay);
}
