#ifndef LED_SEQUENCE_CTX_H
#define LED_SEQUENCE_CTX_H

#include <stdint.h>

#define BLINK_SEQUENCE_MAX_SIZE 13

typedef struct
{
	uint32_t *gpio_pin_sequence;
	uint8_t sequence_size;
	uint8_t next_pin_idx;
	uint8_t curr_pin_idx;
} led_sequence_ctx_t;

/*
	Initializes R, G, B leds;
	Parses sequence (only 'R', 'G' and 'B' characters will be processed, other characters will be ignored);
	And converts this sequence to a valid led blinking sequence;
*/
void led_init_ctx(led_sequence_ctx_t *ctx, uint32_t *blink_queue, const char *blink_sequence);

/*
	Sends on signal to a led according to a sequence order
*/
void led_switch_on_next(led_sequence_ctx_t *ctx);

/*
	Switches on the led which pointed by ctx now
*/
void led_switch_on_current(led_sequence_ctx_t *ctx);

/*
	Switches off currently active led
*/
void led_switch_off(led_sequence_ctx_t *ctx);

#endif