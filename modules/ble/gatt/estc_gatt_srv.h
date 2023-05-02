#ifndef ESTC_BLE_SERVICE_H__
#define ESTC_BLE_SERVICE_H__

#include <stdint.h>
#include <stdbool.h>

#include "ble.h"
#include "sdk_errors.h"

#define ESTC_BLE_MAX_SERVICES 5

typedef struct
{
	uint8_t * uuid128;       /**< Filled by initialer. */
	uint16_t service_handle; /**< Service handle assigned by softdevice. */
	uint8_t uuid_type;       /**< Service UUID type which is set by softdevice. */
} estc_ble_service_t;

/**
 * @brief Initializes ble gatt service
 *
 * @param [in] service - information about service which will be registered
 * 
 * @param [in] service_uuid128 - required 128-bit service UUID
 * 
 * @param [in] service_id - 16-bit service id
 *
 * @retval NRF_ERROR_NO_MEM - No free slots for service UUIDs
 *
 * @retval NRF_ERROR_INVALID_ADDR - Address of the service is invalid
 *
 * @retval NRF_SUCCESS - service was registered successfully
 */
ret_code_t estc_ble_service_init(estc_ble_service_t *service, uint8_t service_uuid128[16], uint16_t service_id);

#endif /* ESTC_SERVICE_H__ */