#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "modules/button/board_button.h"
#include "modules/led/sequence_ctx.h"
#include "nrf_delay.h"

#define BLINK_SEQUENCE          "RGBBGRRRGGBB"

#define LED_ON_TIME_MS  750
#define LED_OFF_TIME_MS 250

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

    /* Toggle LEDs. */
    while (true)
    {
        while (button_is_pressed(button))
        {
            led_switch_off(&leds);

            led_switch_on_next(&leds);
            nrf_delay_ms(LED_ON_TIME_MS);

            if (button_is_pressed(button))
            {
                led_switch_off(&leds);
                nrf_delay_ms(LED_OFF_TIME_MS);
            }
        }
    }
}

/**
 *@}
 **/
