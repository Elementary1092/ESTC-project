#ifndef BUTTON_CONTROLLER_H
#define BUTTON_CONTROLLER_H

#include <stdint.h>
#include <hal/nrf_gpio.h>

typedef enum
{
	BOARD_BUTTON_RESET = NRF_GPIO_PIN_MAP(0, 18),
	BOARD_BUTTON_SW1   = NRF_GPIO_PIN_MAP(1, 6),
} board_button_t;

/* Initialize button input with pull-up resistor */
void button_init(board_button_t button);

/* Determines whether button is pressed or not (since pull-up resistor is used, 0 - means not pressed, otherwise pressed) */
uint32_t button_is_pressed(board_button_t button);

#endif