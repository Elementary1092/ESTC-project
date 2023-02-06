#ifndef MODULES_SYSTICK_CTX_H
#define MODULES_SYSTICK_CTX_H

#include <stdint.h>
#include <stdbool.h>
#include <nrfx_systick.h>

/**
 * @brief Initializes nrfx_systick module.
*/
void timer_systick_ctx_init(void);

/**
 * @brief Used to start countdown.
 * 
 * @attention Supports only one countdown at a time.
*/
void timer_systick_ctx_probe(void);

/**
 * @brief Check whether needed time elapsed since the last countdown start.
 * 
 * @param[in] delay Time supposed to be elapsed in microseconds.
*/
bool timer_systick_ctx_has_time_elapsed(uint32_t delay);

#endif