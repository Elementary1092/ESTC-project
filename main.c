#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "fds.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "bsp_btn_ble.h"
#include "sensorsim.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"

#include "modules/ble/estc_ble.h"
#include "modules/ble/gatt/estc_gatt.h"
#include "modules/ble/gatt/estc_gatt_srv.h"
#include "modules/ble/gatt/estc_gatt_srv_char.h"
#include "modules/ble/gap/advertising.h"
#include "modules/ble/gap/estc_gap.h"
#include "modules/ble/estc_chars/char_with_desc.h"
#include "modules/ble/estc_chars/char_indicated.h"
#include "modules/ble/estc_chars/char_notified.h"
#include "utils/generator/fcyclic_variable.h"
#include "utils/generator/sinusoid.h"

#define DEVICE_NAME "SomeLongName"

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(6000)
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(40000)
#define MAX_CONN_PARAMS_UPDATE_COUNT 3

#define MIN_CONN_INTERVAL 100U
#define MAX_CONN_INTERVAL 200U
#define SLAVE_LATENCY 0
#define CONN_SUP_TIMEOUT 4000U

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static ble_uuid_t m_adv_uuids[] =
{
    {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
    {ESTC_BLE_SERVICE_UUID, BLE_UUID_TYPE_BLE},
};

static estc_ble_service_t m_estc_service =
{
    .service_handle = 0U,
};

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

static void timers_init(void)
{
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

static void gap_params_init(void)
{
    estc_ble_gap_config_t config =
    {
        .device_name = (const uint8_t *)DEVICE_NAME,
        .device_name_len = strlen(DEVICE_NAME),
        .min_conn_interval_ms = MIN_CONN_INTERVAL,
        .max_conn_interval_ms = MAX_CONN_INTERVAL,
        .slave_latency = SLAVE_LATENCY,
        .conn_supplement_timeout_ms = CONN_SUP_TIMEOUT,
    };
    ret_code_t err_code = estc_ble_gap_peripheral_init(config);
    APP_ERROR_CHECK(err_code);
}

static void gatt_init(void)
{
    ret_code_t err_code = estc_ble_gatt_init(estc_ble_gatt_evt_hnd_with_log);
    APP_ERROR_CHECK(err_code);
}

static void services_init(void)
{
    ret_code_t err_code;

    estc_ble_qwr_init();

    err_code = estc_ble_service_init(&m_estc_service);
    APP_ERROR_CHECK(err_code);
    
    err_code = estc_char_with_desc_register(&m_estc_service);
    APP_ERROR_CHECK(err_code);
    
    err_code = estc_char_notified_register(&m_estc_service);
    APP_ERROR_CHECK(err_code);
    estc_ble_add_conn_subscriber(estc_char_notified_start);
    estc_ble_add_disconn_subscriber(estc_char_notified_stop);

    err_code = estc_char_indicated_register(&m_estc_service);
    APP_ERROR_CHECK(err_code);
    estc_ble_add_conn_subscriber(estc_char_indicated_start);
    estc_ble_add_disconn_subscriber(estc_char_indicated_stop);
}

static void ble_stack_init(void)
{
    ret_code_t err_code = estc_ble_stack_init(APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, estc_ble_default_ble_event_handler, NULL);
}

static void conn_params_init(void)
{
    estc_ble_conn_peripheral_cfg_t config =
    {
        .first_conn_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY,
        .next_conn_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY,
        .max_conn_update_count = MAX_CONN_PARAMS_UPDATE_COUNT,
        .conn_evt_handler = estc_ble_conn_params_evt_handler,
        .conn_error_handler = estc_ble_conn_params_err_handler,
    };

    ret_code_t error_code = estc_ble_conn_peripheral_init(&config);
    APP_ERROR_CHECK(error_code);
}

static void buttons_leds_init(void)
{
    ret_code_t err_code;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, estc_bsp_default_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, NULL);
    APP_ERROR_CHECK(err_code);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
    LOG_BACKEND_USB_PROCESS();
}

int main(void)
{
    // Initialize.
    log_init();
    timers_init();
    buttons_leds_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    estc_ble_gap_advertising_init(m_adv_uuids, sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]));
    services_init();
    conn_params_init();

    // Start execution.
    NRF_LOG_INFO("ESTC advertising example started.");
    estc_ble_gap_advertising_start();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}
