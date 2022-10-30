#include <stdbool.h>
#include <stdint.h>

#include "blink/controller.h"
#include "nrf_delay.h"
#include "boards.h"

#define LED_INVERT_DELAY 500
#define ITERATION_DELAY_TIME 3000

#define BOARD_ID_REVERSED 8917

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Configure board. */
    bsp_board_init(BSP_INIT_LEDS);

    /* Initialize blink time controller */
    struct time_controller_t c;
    init_time_controller(&c, BOARD_ID_REVERSED);

    /* Toggle LEDs. */
    while (true)
    {
        for (uint8_t i = next(&c); i > 0; i--)
        {
            for (int j = 0; j < LEDS_NUMBER; j++) {
                bsp_board_led_invert(j);
                nrf_delay_ms(LED_INVERT_DELAY);
                bsp_board_led_invert(j);
                if (i != 1) {
                    nrf_delay_ms(LED_INVERT_DELAY);
                } else {
                    bsp_board_led_off(j);
                }
            }
        }

        nrf_delay_ms(ITERATION_DELAY_TIME);
    }

    destruct_time_controller(&c);
}

/**
 *@}
 **/
