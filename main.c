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
#include "modules/timer/systick_ctx.h"
#include "modules/timer/rtc.h"
#include "modules/pwm/pwm_led.h"

#define BLINK_SEQUENCE  "RGBBGRRRGGBB"

#define LED_PWM_FREQUENCY_HZ 1000

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
    timer_systick_ctx_init();

    board_button_t button = BOARD_BUTTON_SW1;

    button_init(button);

    /* Initializing leds and setting blinking sequence */
    led_sequence_ctx_t leds;
    uint32_t blink_queue[BLINK_SEQUENCE_MAX_SIZE] = {0UL};
    led_init_ctx(&leds, blink_queue, BLINK_SEQUENCE);

    pwm_led_ctx_t pwm_ctx;
    uint32_t pwm_led_seq_queue[PWM_LED_SEQ_SIZE] = {0UL};
    pwm_led_ctx_init(&pwm_ctx, &leds, pwm_led_seq_queue, LED_PWM_FREQUENCY_HZ);

    timer_systick_ctx_probe();
    /* Toggle LEDs. */
    while (true)
    {
        if (button_get_recent_state(button) == BUTTON_PRESSED_TWICE_RECENTLY)
        {
            pwm_led_ctx_freeze_unfreeze();
        }

        pwm_led_ctx_process(&pwm_ctx);

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}

/**
 *@}
 **/
