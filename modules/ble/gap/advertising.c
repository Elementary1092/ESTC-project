#include <ble_advdata.h>
#include <ble_advertising.h>
#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <bsp.h>
#include <app_error.h>
#include <nrf_log.h>

#include "advertising.h"

BLE_ADVERTISING_DEF(m_advertising); /**< Advertising module instance. */

/**
 * @brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
    case BLE_ADV_EVT_FAST:
        NRF_LOG_INFO("Fast advertising.");
        break;

    default:
        break;
    }
}

void estc_ble_gap_advertising_init(ble_uuid_t * adv_uuids, uint16_t adv_uuids_size)
{
    // If adv_uuids == NULL adv_uuids_size should be equal to 0.
    APP_ERROR_CHECK_BOOL(adv_uuids == NULL && adv_uuids_size == 0);

    ret_code_t err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type = BLE_ADVDATA_NO_NAME;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    init.srdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.srdata.uuids_complete.uuid_cnt = adv_uuids_size;
    init.srdata.uuids_complete.p_uuids = adv_uuids;

    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout = APP_ADV_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

void estc_ble_gap_advertising_start(void)
{
    ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}