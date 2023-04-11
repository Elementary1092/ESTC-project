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

#include "modules/ble/service/service.h"
#include "modules/ble/service/characteristic.h"
#include "modules/ble/gap/advertising.h"
#include "modules/ble/ble_helpers.h"
#include "modules/ble/gap/estc_gap.h"
#include "utils/generator/fcyclic_variable.h"
#include "utils/generator/sinusoid.h"

#define DEVICE_NAME "SomeLongName"              /**< Extended name of a device. */
#define MANUFACTURER_NAME "NordicSemiconductor" /**< Manufacturer. Will be passed to Device Information Service. */

#define MIN_CONN_INTERVAL 100U /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL 200U /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY 0        /**< Slave latency. */
#define CONN_SUP_TIMEOUT 4000U /**< Connection supervisory timeout (4 seconds). */

#define ESTC_NOTIFIED_VALUE_DELAY APP_TIMER_TICKS(2500)
#define ESTC_INDICATED_VALUE_DELAY APP_TIMER_TICKS(3500)

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define ESTC_SOME_CHARACTERISTIC_UUID16 0x05b9
#define ESTC_NOTIFIED_CHAR_UUID16 0x06b9
#define ESTC_INDICATED_CHAR_UUID16 0x06ba

static uint8_t some_characteristic_desc[] = {0x73, 0x6f, 0x6d, 0x65, 0x20, 0x64, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67};

NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
APP_TIMER_DEF(m_notification_timer);
APP_TIMER_DEF(m_indication_timer);

static ble_uuid_t m_adv_uuids[] = /**< Universally unique service identifiers. */
{
    {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
    {ESTC_BLE_SERVICE_UUID, BLE_UUID_TYPE_BLE},
};

// static int8_t tx_power_level = 63;

static estc_ble_service_t m_estc_service =
{
    .service_handle = 0U,
};

static uint32_t some_characteristic_value = 32U;

static utils_generator_fcyclic_variable_ctx_t notified_value =
{
    .current_value = 0.0F,
    .is_valid = true,
    .max_value = 250.0F,
    .min_value = -250.F,
    .is_increasing = true,
    .step = 5.0F,
};
static uint16_t notified_value_handle = 0U;

static utils_generator_sinusoid_ctx_t indicated_value =
{
    .angle_factor = 2.0F,
    .angle_step = 1.0F,
    .current_angle = 0.0F,
    .max_angle_value = 250.0F,
    .current_value = 0.0F,
};
static uint16_t indicated_value_handle = 0U;

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

static void ble_notified_value_handler(void *p_ctx)
{
    utils_generator_fcyclic_variable_next(&notified_value);
    uint16_t conn_handle = estc_ble_get_conn_handle();
    uint16_t value_len = sizeof(notified_value.current_value);
    estc_ble_service_characteristic_send_notification(conn_handle, notified_value_handle, (uint8_t *)(&(notified_value.current_value)), &value_len);
}

static void ble_indicated_value_handler(void *p_ctx)
{
    utils_generator_sinusoid_next(&indicated_value);
    uint16_t conn_handle = estc_ble_get_conn_handle();
    uint16_t value_len = sizeof(indicated_value.current_value);
    estc_ble_service_characteristic_send_indication(conn_handle, indicated_value_handle, (uint8_t *)(&(indicated_value.current_value)), &value_len);
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    app_timer_create(&m_notification_timer, APP_TIMER_MODE_REPEATED, ble_notified_value_handler);
    app_timer_create(&m_indication_timer, APP_TIMER_MODE_REPEATED, ble_indicated_value_handler);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
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

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    ret_code_t err_code;

    estc_ble_qwr_init();

    err_code = estc_ble_service_init(&m_estc_service);
    APP_ERROR_CHECK(err_code);

    estc_ble_service_characteristic_config_t dummy_config =
    {
        .characteristic_uuid = ESTC_SOME_CHARACTERISTIC_UUID16,
        .description_string = some_characteristic_desc,
        .description_size = sizeof(some_characteristic_desc),
        .permissions = ESTC_BLE_SERVICE_CHARACTERISTIC_ALLOW_READ | ESTC_BLE_SERVICE_CHARACTERISTIC_ALLOW_WRITE,
        .value = (uint8_t *)(&some_characteristic_value),
        .value_size = sizeof(some_characteristic_value),
        .value_type = ESTC_BLE_SERVICE_CHARACTERISTIC_TYPE_UINT32,
        .value_size_max = sizeof(some_characteristic_value),
        .characteristic_uuid_type = BLE_UUID_TYPE_BLE,
    };

    estc_ble_characteristic_handles_t dummy_handles;

    err_code = estc_ble_service_register_characteristic(&m_estc_service, &dummy_config, &dummy_handles);
    APP_ERROR_CHECK(err_code);

    estc_ble_service_characteristic_config_t notified_config =
    {
        .characteristic_uuid = ESTC_NOTIFIED_CHAR_UUID16,
        .permissions = ESTC_BLE_SERVICE_CHARACTERISTIC_ALLOW_READ | ESTC_BLE_SERVICE_CHARACTERISTIC_ALLOW_NOTIFICATION,
        .value = (uint8_t *)(&notified_value.current_value),
        .value_size = sizeof(notified_value.current_value),
        .value_type = ESTC_BLE_SERVICE_CHARACTERISTIC_TYPE_FLOAT32,
        .value_size_max = sizeof(notified_value.current_value),
        .characteristic_uuid_type = BLE_UUID_TYPE_BLE,
    };

    estc_ble_characteristic_handles_t notified_handles;

    err_code = estc_ble_service_register_characteristic(&m_estc_service, &notified_config, &notified_handles);
    APP_ERROR_CHECK(err_code);

    notified_value_handle = notified_handles.value_handle.service_handle;

    estc_ble_service_characteristic_config_t indicated_config =
    {
        .characteristic_uuid = ESTC_INDICATED_CHAR_UUID16,
        .permissions = ESTC_BLE_SERVICE_CHARACTERISTIC_ALLOW_READ | ESTC_BLE_SERVICE_CHARACTERISTIC_ALLOW_INDICATION,
        .value = (uint8_t *)(&indicated_value.current_value),
        .value_size = sizeof(indicated_value.current_value),
        .value_type = ESTC_BLE_SERVICE_CHARACTERISTIC_TYPE_FLOAT32,
        .value_size_max = sizeof(indicated_value.current_value),
        .characteristic_uuid_type = BLE_UUID_TYPE_BLE,
    };

    estc_ble_characteristic_handles_t indicated_handles;

    err_code = estc_ble_service_register_characteristic(&m_estc_service, &indicated_config, &indicated_handles);
    APP_ERROR_CHECK(err_code);

    indicated_value_handle = indicated_handles.value_handle.service_handle;
}

/**@brief Function for starting timers.
 */
static void application_timers_start(void)
{
    app_timer_start(m_notification_timer, ESTC_NOTIFIED_VALUE_DELAY, NULL);
    app_timer_start(m_indication_timer, ESTC_INDICATED_VALUE_DELAY, NULL);
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, estc_ble_default_ble_event_handler, NULL);
}

/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(void)
{
    ret_code_t err_code;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, estc_bsp_default_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
    LOG_BACKEND_USB_PROCESS();
}

/**@brief Function for application main entry.
 */
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
    estc_ble_conn_params_init();

    // Start execution.
    NRF_LOG_INFO("ESTC advertising example started.");
    application_timers_start();

    estc_ble_gap_advertising_start();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}

/**
 * @}
 */
