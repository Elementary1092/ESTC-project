#ifndef BUTTON_CONTROLLER_H
#define BUTTON_CONTROLLER_H

#include <stdint.h>

struct button_controller_t
{
	uint32_t gpio_pin;
};

/* Initialize button with given port and pin numbers as input with pull-up resistor */
void init_button_controller(struct button_controller_t *, int, int);

/* Determines whether button is pressed or not (since pull-up resistor is used, 0 - means not pressed, otherwise pressed) */
uint32_t is_pressed(struct button_controller_t *);

#endif