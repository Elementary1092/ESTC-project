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
    uint8_t blink_queue[BLINK_QUEUE_MAX_SIZE] = {0};
    init_time_controller(&c, blink_queue, BOARD_ID_REVERSED);

    /* Toggle LEDs. */
    while (true)
    {
        for (uint8_t i = next(&c); i > 0; i--)
        {
            for (int j = 0; j < LEDS_NUMBER; j++) 
            {
                bsp_board_led_invert(j);
                nrf_delay_ms(LED_INVERT_DELAY);
                bsp_board_led_invert(j);
            }
            if (i == 1) 
            {
                bsp_board_leds_off();
            }
        }

        nrf_delay_ms(ITERATION_DELAY_TIME);
    }

    destruct_time_controller(&c);
}

/**
 *@}
 **/
