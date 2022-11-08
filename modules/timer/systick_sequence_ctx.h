/*
	This module wraps operations with systick timer
*/

#ifndef MODULES_SYSTICK_CTX_H
#define MODULES_SYSTICK_CTX_H

#include <stdint.h>
#include <stdbool.h>

#define SYSTICK_CTX_ON_SEQ_SIZE 11

typedef struct
{
	uint32_t *on_seq_us;
	uint32_t seq_cycle_us;
	uint8_t idx;
	bool should_inc;
} systick_sequence_ctx_t;

/*
	systick_ctx_init() initializes systick & context of the delay sequence
*/
void timer_systick_sequence_ctx_init(systick_sequence_ctx_t *ctx, uint32_t *on_seq_queue_us, uint16_t frequency);

/*
	systick_sequence_ctx_has_time_elapsed() checks had the time elapsed according to the provided context
	(specifically according to the delay_sequence which is provided by this context)
*/
bool timer_systick_sequence_ctx_has_time_elapsed(systick_sequence_ctx_t *ctx, bool on_time);

#endif