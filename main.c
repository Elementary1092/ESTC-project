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
#include "modules/gpio/led/led.h"
#include "modules/timer/rtc.h"
#include "modules/hsv/hsv_picker.h"

// Board ID: 7198, so, 7 - Red, 1 - Green, 9 - Blue, 8 - Green (led1) 
#define BLINK_SEQUENCE  "RRRRRRRGBBBBBBBBBYYYYYYYY"

// 360 * 0.98
#define INITIAL_HSV_HUE        353
#define INITIAL_HSV_SATURATION 100
#define INITIAL_HSV_BRIGHTNESS 100

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

    /* Initializing leds */
    led_init_all();

    hsv_picker_init(INITIAL_HSV_HUE, INITIAL_HSV_SATURATION, INITIAL_HSV_BRIGHTNESS);

    /* Toggle LEDs. */
    while (true)
    {
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}

/**
 *@}
 **/
