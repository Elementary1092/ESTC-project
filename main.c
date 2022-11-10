#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include <nrfx_gpiote.h>
#include <nordic_common.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_log_default_backends.h>
#include <nrf_log_backend_usb.h>
#include <app_usbd.h>
#include <app_usbd_serial_num.h>

#include "modules/gpio/button/board_button.h"
#include "modules/gpio/led/sequence_ctx.h"
#include "modules/timer/systick_sequence_ctx.h"
#include "modules/timer/rtc.h"

#define BLINK_SEQUENCE  "RGBBGRRRGGBB"

#define LED_PWM_FREQUENCY_HZ 1000

#define N_CYCLES_ONE_LED 1000
#define PWM_CYCLE_MOVE_NEXT 200

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    nrfx_err_t err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INIT(NULL);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("Starting project");

    timer_rtc_init();

    board_button_t button = BOARD_BUTTON_SW1;

    button_init(button);

    /* Initializing leds and setting blinking sequence */
    led_sequence_ctx_t leds;
    uint32_t blink_queue[BLINK_SEQUENCE_MAX_SIZE] = {0};
    led_init_ctx(&leds, blink_queue, BLINK_SEQUENCE);

    systick_sequence_ctx_t systick_ctx;
    uint32_t systick_delay_sequence_us[SYSTICK_CTX_SEQ_SIZE] = {0};
    timer_systick_sequence_ctx_init(&systick_ctx, systick_delay_sequence_us, LED_PWM_FREQUENCY_HZ);

    bool led_turned_on = false;
    bool is_blink_on = false;
    uint16_t led_ncycles = 0;
    uint16_t pwm_ncycles = 0;

    timer_systick_sequence_ctx_probe(&systick_ctx);
    /* Toggle LEDs. */
    while (true)
    {
        if (pwm_ncycles == 0)
        {
            led_switch_off(&leds);
        }

        if (button_get_recent_state(button) == BUTTON_PRESSED_TWICE_RECENTLY)
        {
            is_blink_on = !is_blink_on;
        }

        if (timer_systick_sequence_ctx_has_time_elapsed(&systick_ctx, led_turned_on))
        {
            if (!led_turned_on)
            {
                if (led_ncycles >= N_CYCLES_ONE_LED - 1 && is_blink_on)
                {
                    led_switch_on_next(&leds);
                    led_ncycles = 0;
                }
                else
                {
                    led_switch_on_current(&leds);
                    led_ncycles = (uint32_t)((led_ncycles + 1) % N_CYCLES_ONE_LED);
                }
                led_turned_on = true;
            }
            else
            {
                led_switch_off(&leds);
                led_turned_on = false;

                if (is_blink_on && pwm_ncycles >= PWM_CYCLE_MOVE_NEXT)
                {
                    timer_systick_sequence_ctx_next(&systick_ctx);
                    pwm_ncycles = 0;
                }
            }
            timer_systick_sequence_ctx_probe(&systick_ctx);
            pwm_ncycles++;
        }

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}

/**
 *@}
 **/
