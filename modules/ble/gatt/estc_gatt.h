#ifndef ESTC_BLE_GATT_H_
#define ESTC_BLE_GATT_H_

#include <nrf_ble_gatt.h>
#include <app_error.h>

/**
 * @brief Signature of a handler of GATT module events.
*/
typedef void (*estc_ble_gatt_evt_handler_t)(nrf_ble_gatt_t *gatt_inst, nrf_ble_gatt_evt_t *gatt_evt);

/**
 * @brief Initialize GATT module.
 * 
 * @param[in] evt_handler - GATT event handler.
 * 
 * @retval NRF_SUCCESS - successfully initialized GATT module.
*/
ret_code_t estc_ble_gatt_init(estc_ble_gatt_evt_handler_t evt_handler);

/**
 * @brief Default GATT event handler. Logs MTU change and data length update.
*/
void estc_ble_gatt_evt_hnd_with_log(nrf_ble_gatt_t *gatt_inst, nrf_ble_gatt_evt_t *gatt_evt);

#endif // ESTC_BLE_GATT_H_