/*
	This module wraps operations with systick timer
*/

#ifndef MODULES_SYSTICK_CTX_H
#define MODULES_SYSTICK_CTX_H

#include <stdint.h>
#include <stdbool.h>
#include <nrfx_systick.h>

/*
	systick_ctx_init() initializes systick
*/
void timer_systick_ctx_init(void);

/*
	Start countdown
*/
void timer_systick_ctx_probe(void);

/*
	Checks had the time elapsed according to the provided delay
*/
bool timer_systick_ctx_has_time_elapsed(uint32_t delay);

#endif