#ifndef ESTC_BLE_SERVICE_H__
#define ESTC_BLE_SERVICE_H__

#include <stdint.h>

#include "ble.h"
#include "sdk_errors.h"

// UUID: 0dc1xxxx-9959-436f-9bd4-dd1d358b958d
/**
 * BLE service base UUID without 12th and 13th octet
*/
#define ESTC_BLE_BASE_UUID { 0x8D, 0x95, 0x8B, 0x35, 0x1D, 0xDD, 0xD4, 0x9B, 0x6F, 0x43, 0x59, 0x99, 0x00, 0x00, 0xC1, 0x0D }

#define ESTC_BLE_SERVICE_UUID 0x2455 /**< 12th and 13th octet of ESTC_BLE_BASE_UUID */

typedef struct
{
	uint16_t service_handle;
} ble_estc_service_t;

/**
 * @brief Initializes ble gatt service
 * 
 * @param [in] service - information about service which will be registered
 * 
 * @retval NRF_ERROR_NO_MEM - No free slots for service UUIDs
 * 
 * @retval NRF_ERROR_INVALID_ADDR - Address of the service is invalid
 * 
 * @retval NRF_SUCCESS - service was registered successfully
*/
ret_code_t estc_ble_service_init(ble_estc_service_t *service);

#endif /* ESTC_SERVICE_H__ */