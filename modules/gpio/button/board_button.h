#ifndef BUTTON_CONTROLLER_H
#define BUTTON_CONTROLLER_H

#include <stdint.h>
#include <nrfx_gpiote.h>

#define BUTTON_RECENT_STATES_NUMBER 4

/**
 * @brief Available buttons on the board.
*/
typedef enum
{
	BOARD_BUTTON_RESET = NRF_GPIO_PIN_MAP(0, 18), /**< Reset button of pca10059. */
	BOARD_BUTTON_SW1   = NRF_GPIO_PIN_MAP(1, 6),  /**< Switch button of pca10059. */
} board_button_t;

/**
 * @brief Possible recent actions on the button.
*/
typedef enum
{
	BUTTON_UNKNOWN_RECENT_STATE = 0,
	BUTTON_NOT_PRESSED_RECENTLY = 1,
	BUTTON_PRESSED_ONCE_RECENTLY = 2,
	BUTTON_PRESSED_TWICE_RECENTLY = 3,
} button_recent_state_t;

/**
 * @brief Handler of recent action on the button.
 * 
 * @details Should be lightweight function because may be called frequently.
*/
typedef void (*button_clicks_subscriber_t)(void);

/**
 * @brief Handler of holding of the button. Checked every 30 ms.
 * 
 * @details Should be lightweight function because may be called frequently.
*/
typedef void (*button_hold_subscriber_t)(void);

/**
 * @brief Inialize particular button.
 * 
 * @details Initialize button input with pull-up resistor.
 * 
 * @attention Requires nrfx_gpiote to be initialized.
 * 
 * @param[in] button Button which should be initialized.
*/
void button_init(board_button_t button);

/**
 * @brief   Information about whether button is pressed or not.
 * 
 * @details Determines whether button is pressed or not 
 *          (since pull-up resistor is used, 0 - means not pressed, otherwise pressed).
 * 
 * @param[in] button Pin of a button where state should be taken.
 * 
 * @retval 0         Is not pressed
 * @retval otherwise Is pressed
*/
uint32_t button_is_pressed(board_button_t button);

/**
 * @brief     Number of recent clicks on the button.
 * 
 * @details   Number of clicks up to 2 in the last 300 ms before the last click.
 * 
 * @attention Works only with BOARD_BUTTON_SW1.
 * 
 * @param[in] button Button pin number.
 * 
 * @retval    @ref BUTTON_UNKNOWN_RECENT_STATE Number of clicks > 2. 
*/
button_recent_state_t button_get_recent_state(board_button_t button);

/**
 * @brief Add recent state handler of SW1 button.
 * 
 * @param[in] state   State which will trigger the handler.
 * @param[in] handler Function which will be triggered.
*/
void button_subscribe_to_SW1_state(button_recent_state_t state, button_clicks_subscriber_t handler);

/**
 * @brief Add handler of hold of the SW1 button.
 * 
 * @param[in] handler Function which will be triggered on the hold.
*/
void button_subscribe_to_SW1_hold(button_hold_subscriber_t handler);

#endif