#ifndef ESTC_BLE_GAP_BOND_H_
#define ESTC_BLE_GAP_BOND_H_

#include <stdint.h>
#include <stdbool.h>
#include <nrf_ble_qwr.h>

#define ESTC_BLE_GAP_BOND_MEM_BUFF_SIZE 512
#define SEC_PARAM_BOND                  1                                       //!< Perform bonding.
#define SEC_PARAM_MITM                  0                                       //!< Man In The Middle protection not required.
#define SEC_PARAM_LESC                  0                                       //!< LE Secure Connections not enabled.
#define SEC_PARAM_KEYPRESS              0                                       //!< Keypress notifications not enabled.
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    //!< No I/O capabilities.
#define SEC_PARAM_OOB                   0                                       //!< Out Of Band data not available.
#define SEC_PARAM_MIN_KEY_SIZE          7                                       //!< Minimum encryption key size.
#define SEC_PARAM_MAX_KEY_SIZE          16                                      //!< Maximum encryption key size.

/**
 * @brief Initialize bond managment ble service and peer manager.
*/
void estc_ble_gap_bond_init(void);

/**
 * @brief Delete unused bonds (currently deletes all saved bonds).
*/
void estc_ble_gap_bond_delete_unused(void);

/**
 * @brief Callback which may be called by ble write queue on some events.
*/
uint16_t estc_ble_gap_bond_qwr_evt_handler(nrf_ble_qwr_t * p_qwr, nrf_ble_qwr_evt_t * evt);

bool estc_ble_gap_is_conn_bonded(uint16_t conn_handle);

#endif // ESTC_BLE_GAP_BOND_H_