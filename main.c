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
#include <app_usbd_cdc_acm.h>

#include "modules/gpio/button/board_button.h"
#include "modules/gpio/led/led.h"
#include "modules/timer/rtc.h"
#include "modules/hsv/hsv_picker.h"
#include "modules/hsv/cli/cli.h"
#include "modules/cdc_acm/cdc_acm_cli.h"

// Board ID: 7198, so, 7 - Red, 1 - Green, 9 - Blue, 8 - Green (led1) 
#define BLINK_SEQUENCE  "RRRRRRRGBBBBBBBBBYYYYYYYY"

// 360 * 0.98
#define INITIAL_HSV_HUE        353.0F
#define INITIAL_HSV_SATURATION 100.0F
#define INITIAL_HSV_BRIGHTNESS 100.0F

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Initializing gpiote which is used in button module */
    nrfx_err_t err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);

    /* Initializing logs module */
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("Starting project");

    /* Initializing rtc timer. This kind of timer is used in button module */
    timer_rtc_init();

    board_button_t button = BOARD_BUTTON_SW1;

    button_init(button);

    /* Initializing leds */
    led_init_all();

    /* Initializing hsv shade picker */
    hsv_picker_init(INITIAL_HSV_HUE, INITIAL_HSV_SATURATION, INITIAL_HSV_BRIGHTNESS);

#if ESTC_USB_CLI_ENABLED
    cdc_acm_cli_init();
    cdc_acm_add_handler(CDC_ACM_CLI_USB_RX_NEW_LINE, hsv_cli_exec_command);
#endif

    /* Making hsv_picker_next_mode function to be called on double click of a button */
    button_subscribe_to_SW1_state(BUTTON_PRESSED_TWICE_RECENTLY, hsv_picker_next_mode);

    /* Making hsv_picker_edit_param function to be called on hold of a SW1 button */
    button_subscribe_to_SW1_hold(hsv_picker_edit_param);

    /* Toggle LEDs. */
    while (true)
    {
    #if ESTC_USB_CLI_ENABLED
        while (app_usbd_event_queue_process())
        {

        }
    #else
        __WFI();
    #endif

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}

/**
 *@}
 **/
