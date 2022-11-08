/*
	This module wraps operations with systick timer
*/

#ifndef MODULES_SYSTICK_CTX_H
#define MODULES_SYSTICK_CTX_H

#include <stdint.h>
#include <stdbool.h>
#include <nrfx_systick.h>

#define SYSTICK_CTX_SEQ_SIZE 11

typedef struct
{
	uint32_t *on_seq_us;
	nrfx_systick_state_t state;
	uint32_t seq_cycle_us;
	uint8_t idx;
	bool should_inc;
} systick_sequence_ctx_t;

/*
	systick_ctx_init() initializes systick & context of the delay sequence
*/
void timer_systick_sequence_ctx_init(systick_sequence_ctx_t *ctx, uint32_t *on_seq_queue_us, uint16_t frequency);

/*
	Start countdown
*/
void timer_systick_sequence_ctx_probe(systick_sequence_ctx_t *ctx);

/*
	systick_sequence_ctx_has_time_elapsed() checks had the time elapsed according to the provided context
	(specifically according to the delay_sequence which is provided by this context)
*/
bool timer_systick_sequence_ctx_has_time_elapsed(systick_sequence_ctx_t *ctx, bool invert);

/*
	Move to the next cycle in pwm
*/
void timer_systick_sequence_ctx_next(systick_sequence_ctx_t *ctx);

#endif