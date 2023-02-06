#ifndef ESTC_PROJECT_PWM_INDIVIDUAL_H
#define ESTC_PROJECT_PWM_INDIVIDUAL_H

#include <nrfx_pwm.h>
#include <nrf_drv_pwm.h>
#include <app_util_platform.h>
#include <stdint.h>

/**
 * @brief Type of custom event handler.
 *        This handler will be called only when pwm is initialized by this module.
 *        By default, event handler does nothing.
 */
typedef void (*pwm_individual_event_handler_t)(nrfx_pwm_evt_type_t);

/**
 * @brief Declares possible output pins.
 *        Should be used on registering output pins and setting output pins values.
 */
typedef enum
{
	PWM_INDIVIDUAL_OUTPUT_PIN0,
	PWM_INDIVIDUAL_OUTPUT_PIN1,
	PWM_INDIVIDUAL_OUTPUT_PIN2,
	PWM_INDIVIDUAL_OUTPUT_PIN3,
} pwm_individual_output_pin_order_t;

/**
 * @brief Defines configuration of PWM which is initialized in individual mode.
 */
typedef struct
{
	nrf_pwm_clk_t clock_base;	 /**<Defines frequency of PWM. */
	nrf_pwm_mode_t count_mode;	 /**< May be NRF_PWM_MODE_UP(edge aligned) or NRF_PWM_MODE_UP_AND_DOWN(center aligned). */
	uint32_t sequence_repeats;	 /**< Defines how many times one sequence should be repeated. */
	uint32_t sequence_end_delay; /**< Defines delay of raising DONE event after sequence is completed. */
	uint16_t top_value;			 /**< Defines value which is considered as 100% of PWM. */
} pwm_individual_config_t;

/**
 * @brief   Registers output pins.
 *
 * @details Binds pin_number to pin_order
 *          which enables changing value at some pin by its binded pin_order.
 *
 * @param[in] pin_order  Some value from pwm_individual_pin_order_t.
 *                       If the value is not from this enum, output pin will not be registered.
 * @param[in] pin_number Pin which should be binded to an order.
 *
 * @attention All output pins should be registered before calling pwm_individual_init.
 */
void pwm_individual_register_output_pin(pwm_individual_output_pin_order_t pin_order, uint8_t pin_number);

/**
 * @brief   Initializes pwm module in individual mode.
 *
 * @details Sets up configuration of pwm by setting IRQ_PRIORITY to LOWEST and count_mode to AUTO.
 *          Sets up sequence configuration.
 *          Registers pwm event handler.
 * 
 * @attention Support handling only one pwm instance.
 *
 * @param[in] config        Configuration of pwm module.
 *
 * @param[in] event_handler Event handler defined by the user.
 */
void pwm_individual_init(pwm_individual_config_t *config, pwm_individual_event_handler_t event_handler);

/**
 * @brief   Terminates PWM playback.
 *
 * @details Stops playback;
 *          clears pwm sequence, sequence config, and output pins binding;
 *          and unregisters event handler.
 */
void pwm_individual_stop(void);

/**
 * @brief Changes pwm value on certain output pin.
 * 
 * @param[in] pin_order If the value is not from pwm_individual_pin_order, function immediately returns.
 * 
 * @param[in] new_value -UINT16_MAX - sets maximum value.
 *                      -default - sets the value on a pin to a particular value.
*/
void pwm_individual_set_output_pin_value(pwm_individual_output_pin_order_t pin_order, uint16_t new_value);

/**
 * @brief Restarts pwm playback by calling nrfx_pwm_simple_playback()
 * 
 * @param[in] repeat_times Sets how many times should specified playback will be repeated.
 *            @attention Should not be equal to 0.
*/
void pwm_individual_start_playback(uint16_t repeat_times);

#endif