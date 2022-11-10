#ifndef BUTTON_CONTROLLER_H
#define BUTTON_CONTROLLER_H

#include <stdint.h>
#include <nrfx_gpiote.h>

typedef enum
{
	BOARD_BUTTON_RESET = NRF_GPIO_PIN_MAP(0, 18),
	BOARD_BUTTON_SW1   = NRF_GPIO_PIN_MAP(1, 6),
} board_button_t;

typedef enum
{
	BUTTON_UNKNOWN_RECENT_STATE = 0,
	BUTTON_NOT_PRESSED_RECENTLY = 1,
	BUTTON_PRESSED_ONCE_RECENTLY = 2,
	BUTTON_PRESSED_TWICE_RECENTLY = 3,
} button_recent_state_t;

/* 
	Initialize button input with pull-up resistor.
	Requires nrfx_gpiote to be initialized.
*/
void button_init(board_button_t button);

/* Determines whether button is pressed or not (since pull-up resistor is used, 0 - means not pressed, otherwise pressed) */
uint32_t button_is_pressed(board_button_t button);

button_recent_state_t button_get_recent_state(board_button_t button);

#endif