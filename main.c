#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "modules/button/board_button.h"
#include "modules/led/led_sequence_ctx.h"
#include "modules/timer/systick_sequence_ctx.h"

#define BLINK_SEQUENCE  "RGBBGRRRGGBB"

#define LED_PWM_FREQUENCY_HZ 1000

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t button = BOARD_BUTTON_SW1;

    button_init(button);

    /* Initializing leds and setting blinking sequence */
    led_sequence_ctx_t leds;
    uint32_t blink_queue[BLINK_SEQUENCE_MAX_SIZE] = {0};
    led_init_ctx(&leds, blink_queue, BLINK_SEQUENCE);

    systick_sequence_ctx_t systick_ctx;
    uint32_t systick_delay_sequence_us[SYSTICK_CTX_ON_SEQ_SIZE] = {0};
    timer_systick_sequence_ctx_init(&systick_ctx, systick_delay_sequence_us, LED_PWM_FREQUENCY_HZ);

    bool led_turned_on = false;
    /* Toggle LEDs. */
    while (true)
    {
        while (button_is_pressed(button))
        {
            led_switch_off(&leds);

            if (timer_systick_sequence_ctx_has_time_elapsed(&systick_ctx, led_turned_on))
            {
                if (!led_turned_on)
                {
                    led_switch_on_next(&leds);
                    led_turned_on = true;
                }
                else
                {
                    led_switch_off(&leds);
                    led_turned_on = false;
                }
            }
        }
    }
}

/**
 *@}
 **/
