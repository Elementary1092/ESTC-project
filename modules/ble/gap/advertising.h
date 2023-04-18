#ifndef ESTC_BLE_GAP_ADVERTISING_H_
#define ESTC_BLE_GAP_ADVERTISING_H_

#include <ble.h>

#define APP_ADV_INTERVAL 300    /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_DURATION 18000  /**< The advertising duration (180 seconds) in units of 10 milliseconds. */
#define APP_BLE_OBSERVER_PRIO 3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1  /**< A tag identifying the SoftDevice BLE configuration. */

/**
 * @brief Function for initializing the Advertising functionality.
*/
void estc_ble_gap_advertising_init(ble_uuid_t * adv_uuids, uint16_t adv_uuids_size);

/**
 * @brief Function for starting advertising.
 */
void estc_ble_gap_advertising_start(void);

#endif // ESTC_BLE_GAP_ADVERTISING_H_
