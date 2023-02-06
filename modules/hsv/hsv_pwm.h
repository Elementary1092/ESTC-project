#ifndef ESTC_HSV_PWM_H
#define ESTC_HSV_PWM_H

#include <stdint.h>

/**
 * @brief Initializes pwm module in a manner required by an hsv module.
 *
 * @details Uses RED, GREEN, BLUE, and LED2 (YELLOW) LEDs.
 *          Initializes pwm module with 1MHz clock_base, top_value = 2500,
 *          in UP counting mode without repetion of sequence and without sequence delay.
 *          Sets up indicator in CONSTANT MODE.
 *          Immediatelly returns if pwm is already initialized.
 */
void hsv_pwm_init(void);

/**
 * @brief Sets up values on red, green, and blue pins of the pwm.
 */
void hsv_pwm_update_rgb_channels(uint16_t red, uint16_t green, uint16_t blue);

/**
 * @brief Starts new playback with updated values.
 */
void hsv_pwm_restart_playback(void);

/**
 * @brief Sets the value on indicator pin of pwm to the passed value and preserves this value.
 *
 * @param[in] value New value on indicator pin.
 */
void hsv_pwm_indicator_blink_constantly(uint16_t value);

/**
 * @brief Makes an indicator blink smoothly by generating new sinusoid value after every playback.
 * 
 * @param[in] blinking_factor Defines how fast does indicator blink.
 * 
 * @param[in] starting_value Defines starting value of the argument of sin() function.
 */
void hsv_pwm_indicator_blink_smoothly(float blinking_factor, float starting_value);

#endif