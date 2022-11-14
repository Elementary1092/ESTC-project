#ifndef PWM_LED_H
#define PWM_LED_H

#include <stdint.h>
#include "sequence_ctx.h"

#define PWM_LED_SEQ_SIZE 11

typedef struct
{
	led_sequence_ctx_t *led_ctx;
	uint32_t *on_delay_seq;
	uint32_t cycle_us;
	uint32_t led_ncycles;
	uint32_t ncycles;
	uint8_t idx;
	bool should_invert;
	bool should_inc_idx;
	bool should_move_next_led;
} pwm_led_ctx_t;

void pwm_led_ctx_init(pwm_led_ctx_t *c, led_sequence_ctx_t *led_ctx, uint32_t *sequence, uint16_t frequency);

void pwm_led_ctx_process(pwm_led_ctx_t *c);

void pwm_led_ctx_freeze_unfreeze(void);

#endif