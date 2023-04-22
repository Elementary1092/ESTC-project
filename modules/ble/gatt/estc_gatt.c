#include <nrf_ble_gatt.h>
#include <nrf_sdh_ble.h>
#include <nrf_log.h>
#include "estc_gatt.h"

NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */

ret_code_t estc_ble_gatt_init(estc_ble_gatt_evt_handler_t evt_handler)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, (nrf_ble_gatt_evt_handler_t)evt_handler);

    return err_code;
}

void estc_ble_gatt_evt_hnd_with_log(nrf_ble_gatt_t *gatt_inst, nrf_ble_gatt_evt_t *gatt_evt)
{
    switch (gatt_evt->evt_id)
    {
    case NRF_BLE_GATT_EVT_ATT_MTU_UPDATED:
        NRF_LOG_INFO(
            "Updated attribute MTU by %hu connection. New value: %hu",
            gatt_evt->conn_handle,
            gatt_evt->params.att_mtu_effective);
        break;

    case NRF_BLE_GATT_EVT_DATA_LENGTH_UPDATED:
        NRF_LOG_INFO(
            "Updated data length by %hu connection. New value: %hu",
            gatt_evt->conn_handle,
            gatt_evt->params.data_length);
        break;

    default:
        NRF_LOG_INFO("Unknown gatt event: %hu", gatt_evt->evt_id);
        break;
    }
}
