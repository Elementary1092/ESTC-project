#ifndef ESTC_BLE_WRITE_MNGR_H_
#define ESTC_BLE_WRITE_MNGR_H_

#include <stdint.h>
#include <sdk_errors.h>

#define ESTC_BLE_WRITE_MNGR_MAX_SUBS 5

typedef void (*estc_ble_write_mngr_subscriber_t)(uint16_t conn_handle, uint8_t * data, uint16_t offset, uint16_t len);

ret_code_t estc_ble_write_mngr_subscribe(uint16_t value_handle, estc_ble_write_mngr_subscriber_t handler);

void estc_ble_write_mngr_handle(uint16_t conn_handle, uint16_t value_handle, uint8_t * data, uint16_t offset, uint16_t len);

#endif // ESTC_BLE_WRITE_MNGR_H_