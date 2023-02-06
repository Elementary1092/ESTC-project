#ifndef LED_SEQUENCE_CTX_H
#define LED_SEQUENCE_CTX_H

#include <stdint.h>

#define BLINK_SEQUENCE_MAX_SIZE 40

/**
 * @brief Defines led blinking order.
*/
typedef struct
{
	uint32_t *gpio_pin_sequence; /**< Should be defined elsewhere and lifetime should be >= this structure's lifetime */
	uint8_t sequence_size;       /**< Size of a gpio_pin_sequence */
	uint8_t next_pin_idx;        /**< internal; Index of the next pin. Should not be changed elsewhere. */
	uint8_t curr_pin_idx;        /**< internal; Index of currently used pin. Should not be changed elsewhere. */
} led_sequence_ctx_t;

/**
 * @brief   Initialized led_sequence_ctx_t.
 * 
 * @details Initializes R, G, B leds,
 *          Parses sequence (only 'R', 'G' and 'B' characters will be processed, other characters will be ignored),
 *          and converts this sequence to a valid led blinking sequence.
 * 
 * @param[out] ctx            pointer to the structure which will be filled up by this function.
 * 
 * @param[out] blink_queue    buffer where gpio_pin_sequence will be stored.
 * 
 * @param[in]  blink_sequence sequence which defines the order of blinking. 
*/
void led_init_ctx(led_sequence_ctx_t *ctx, uint32_t *blink_queue, const char *blink_sequence);

/**
 * @brief Sends on signal to a next led in a blinking sequence.
 * 
 * @param[out] ctx Defines blinking sequence. 
*/
void led_switch_on_next(led_sequence_ctx_t *ctx);

/**
 * @brief Switches on the current led in a sequence.
 * 
 * @param[out] ctx Defines blinking sequence. 
*/
void led_switch_on_current(led_sequence_ctx_t *ctx);

/**
 * @brief Switches off currently active led.
 * 
 * @param[out] ctx Defines blinking sequence.
*/
void led_switch_off(led_sequence_ctx_t *ctx);

#endif