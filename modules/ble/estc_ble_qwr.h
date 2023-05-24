#ifndef ESTC_BLE_QWR_H_
#define ESTC_BLE_QWR_H_

#include <nrf_ble_qwr.h>
#include <stdint.h>

#define ESTC_BLE_QWR_MEM_BUFF_SIZE 512

typedef uint16_t (*estc_ble_qwr_evt_handler_t)(nrf_ble_qwr_t * p_qwr, nrf_ble_qwr_evt_t * p_evt);

/**
 * @brief Initialize write queue of ble.
 * 
 * @param [in] evt_handler - handler of events of the queue.
*/
void estc_ble_qwr_init(estc_ble_qwr_evt_handler_t evt_handler);

/**
 * @brief Get pointer to an instance of ble write queue.
*/
nrf_ble_qwr_t * estc_ble_qwr_get(void);

#endif // ESTC_BLE_QWR_H_