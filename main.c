#include <stdbool.h>
#include <string.h>

#include "blink/button_controller.h"
#include "blink/led_controller.h"
#include "nrf_delay.h"

#define SW1_PORT 1
#define SW1_PIN 6

#define BLINK_SEQUENCE "RGBBGRRRGGBB"

#define LED_ON_TIME  750
#define LED_OFF_TIME 250

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Initializing SW1 button */
    struct button_controller_t button;
    init_button_controller(&button, SW1_PORT, SW1_PIN);

    /* Initializing leds and setting blinking sequence */
    struct led_controller_t leds;
    init_led_controller(&leds, BLINK_SEQUENCE, ((uint8_t)strlen(BLINK_SEQUENCE)));

    /* Toggle LEDs. */
    while (true)
    {
        while (is_pressed(&button))
        {
            switch_on_next_led(&leds);
            nrf_delay_ms(LED_ON_TIME);

            if (is_pressed(&button))
            {
                switch_off_led(&leds);
                nrf_delay_ms(LED_OFF_TIME);
            }
        }
    }

    free_led_controller(&leds);
}

/**
 *@}
 **/
