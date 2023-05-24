#ifndef ESTC_BLE_WRITE_MNGR_H_
#define ESTC_BLE_WRITE_MNGR_H_

#include <stdint.h>
#include <sdk_errors.h>

#define ESTC_BLE_WRITE_MNGR_MAX_SUBS 5

/**
 * @brief Type defining handler of an write to the BLE characteristic value.
*/
typedef void (*estc_ble_write_mngr_subscriber_t)(uint16_t conn_handle, uint8_t * data, uint16_t offset, uint16_t len);

/**
 * @brief Save handler as a handler of write event on some value_handle.
 * 
 * @param[in] value_handle handle of a characteristic value.
 * 
 * @param[in] handler callback which will be called on write to the characteristic value (callback may reject write request).
 * 
 * @retval NRF_ERROR_STORAGE_FULL - reached maximum number of subscriber.
 * 
 * @retval NR_SUCCESS - successfuly saved handler as callback on write to the value.
*/
ret_code_t estc_ble_write_mngr_subscribe(uint16_t value_handle, estc_ble_write_mngr_subscriber_t handler);

/**
 * @brief Call callback of a value handle.
 * 
 * @param[in] conn_handle - ble connection which sent write request or command.
 * 
 * @param[in] value_handle - value handle which is a subject of write.
 * 
 * @param[in] data - data which should be written.
 * 
 * @param[in] offset - the index of the first byte which should be rewritten.
 * 
 * @param[in] len - length of the data sent with write request or command.
*/
void estc_ble_write_mngr_handle(uint16_t conn_handle, uint16_t value_handle, uint8_t * data, uint16_t offset, uint16_t len);

#endif // ESTC_BLE_WRITE_MNGR_H_