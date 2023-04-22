#ifndef ESTC_BLE_CHAR_WITH_DESC_H_
#define ESTC_BLE_CHAR_WITH_DESC_H_

#include <app_error.h>
#include "modules/ble/gatt/estc_gatt_srv.h"

#define ESTC_SOME_CHARACTERISTIC_UUID16 0x05b9

ret_code_t estc_char_with_desc_register(estc_ble_service_t * service);

#endif // ESTC_BLE_CHAR_WITH_DESC_H_