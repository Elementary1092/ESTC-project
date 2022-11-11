#include <app_timer.h>
#include <stdbool.h>
#include <nrf_log.h>
#include <nrfx.h>
#include "pwm_led.h"
#include "systick_ctx.h"

#define MICROSECONDS_IN_SECOND      1000000
#define PWM_LED_SEQ_CTX_INITIAL_IDX 0

#define N_CYCLES_ONE_LED 1000
#define PWM_CYCLE_MOVE_NEXT 200

#define CHANGE_IS_FREEZED_DELAY APP_TIMER_TICKS(300)

APP_TIMER_DEF(change_freezed_timer);

static bool volatile is_freezed = true;

static void change_freezed_handler(void *p_context)
{
	NRF_LOG_INFO("Freezing/unfreezing led pwm");
	is_freezed = !is_freezed;
}

void pwm_led_ctx_init(pwm_led_ctx_t *ctx, led_sequence_ctx_t *led_ctx, uint32_t *on_seq_queue_us, uint16_t frequency)
{
	NRFX_ASSERT(ctx != NULL);
	NRFX_ASSERT(led_ctx != NULL);
	NRFX_ASSERT(on_seq_queue_us != NULL);

	ctx->on_delay_seq = on_seq_queue_us;
	ctx->should_inc_idx = PWM_LED_SEQ_CTX_INITIAL_IDX < PWM_LED_SEQ_SIZE;
	ctx->should_invert = false;
	ctx->idx = PWM_LED_SEQ_CTX_INITIAL_IDX;
	ctx->led_ctx = led_ctx;

	uint32_t cycle_step_us = (MICROSECONDS_IN_SECOND / ((uint32_t)frequency)) / ((uint32_t)(PWM_LED_SEQ_SIZE - 1));
	uint32_t on_delay_us = 0;
	for (ssize_t i = 0; i < PWM_LED_SEQ_SIZE; i++)
	{
		ctx->on_delay_seq[i] = on_delay_us;
		on_delay_us += cycle_step_us;
	}

	ctx->cycle_us = ctx->on_delay_seq[PWM_LED_SEQ_SIZE - 1];

	app_timer_create(&change_freezed_timer, APP_TIMER_MODE_SINGLE_SHOT, change_freezed_handler);
}

void pwm_led_ctx_process(pwm_led_ctx_t *ctx)
{
	uint32_t delay = ctx->on_delay_seq[ctx->idx];
	if (ctx->should_invert)
	{
		delay = ctx->cycle_us - delay;
	}

	if (ctx->ncycles == 0)
	{
		led_switch_off(ctx->led_ctx);
	}

	if (timer_systick_ctx_has_time_elapsed(delay))
	{
		if (!(ctx->should_invert))
		{
			if (ctx->led_ncycles >= N_CYCLES_ONE_LED && !is_freezed)
			{
				led_switch_on_next(ctx->led_ctx);
				ctx->led_ncycles = 0UL;
			}
			else
			{
				led_switch_on_current(ctx->led_ctx);
				ctx->led_ncycles += 1UL;
			}
			ctx->should_invert = true;
		}
		else
		{
			led_switch_off(ctx->led_ctx);
			ctx->should_invert = false;

			if (!is_freezed && ctx->ncycles >= PWM_CYCLE_MOVE_NEXT)
			{
				if (ctx->idx == 0)
				{
					ctx->should_inc_idx = true;
				}
				else if (ctx->idx >= PWM_LED_SEQ_SIZE - 1)
				{
					ctx->should_inc_idx = false;
				}

				if (ctx->should_inc_idx)
				{
					ctx->idx++;
				}
				else
				{
					ctx->idx--;
				}

				ctx->ncycles = 0;
			}
		}
		timer_systick_ctx_probe();
		ctx->ncycles++;
	}
}

void pwm_led_ctx_freeze_unfreeze(void)
{
	app_timer_stop(change_freezed_timer);
	app_timer_start(change_freezed_timer, CHANGE_IS_FREEZED_DELAY, NULL);
}
