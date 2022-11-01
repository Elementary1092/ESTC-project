#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <stdint.h>

struct led_controller_t
{
	uint32_t *gpio_pin_sequence;
	uint8_t sequence_size;
	uint8_t pin_idx;
};

/* 
	Initializes R, G, B leds; 
	Parses sequence (only 'R', 'G' and 'B' characters will be processed, other characters will be ignored);
	And converts this sequence to a valid led blinking sequence;
*/
void init_led_controller(struct led_controller_t *, const char *, uint8_t);

/*
	Sends on signal to a led according to a sequence order
*/
void switch_on_next_led(struct led_controller_t *);

/*
	Switches off currently active led
*/
void switch_off_led(struct led_controller_t *);

/*
	malloc() is used to initialize the led_controller_t. 
	So, this function is used to clear allocated memory.
*/
void free_led_controller(struct led_controller_t *);

#endif