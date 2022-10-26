#include <stdbool.h>
#include <stdint.h>

#include "blink/controller.h"
#include "nrf_delay.h"
#include "boards.h"

#define LED_ON_TIME 500
#define LED_OFF_TIME 300
#define ITERATION_DELAY_TIME 3000

#define BOARD_ID 7198

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Configure board. */
    bsp_board_init(BSP_INIT_LEDS);

    /* Initialize blink time controller */
    struct time_controller_t c;
    init_time_controller(&c, BOARD_ID);

    /* Toggle LEDs. */
    while (true)
    {
        for (uint8_t i = next(&c); i > 0; i--)
        {
            bsp_board_leds_on();
            nrf_delay_ms(LED_ON_TIME);
            bsp_board_leds_off();
            nrf_delay_ms(LED_OFF_TIME);
        }

        nrf_delay_ms(ITERATION_DELAY_TIME);
    }

    destruct_time_controller(&c);
}

/**
 *@}
 **/
