#include "modules/ble/service/service.h"

#include "app_error.h"
#include "nrf_log.h"

#include "ble.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"

static const ble_uuid128_t p_vs_uuid = 
{
	.uuid128 = ESTC_BLE_BASE_UUID,
};

static uint8_t p_vs_uuid_type = BLE_UUID_TYPE_UNKNOWN;

ret_code_t estc_ble_service_init(ble_estc_service_t *service)
{
	ret_code_t error_code = NRF_SUCCESS;

	ble_uuid_t service_uuid = 
	{
		.uuid = ESTC_BLE_SERVICE_UUID,
		.type = BLE_UUID_TYPE_VENDOR_BEGIN,
	};
	
	error_code = sd_ble_uuid_vs_add(&p_vs_uuid, &p_vs_uuid_type);
	if (error_code != NRF_SUCCESS)
	{
		return error_code;
	}

	error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &service->service_handle);

	return error_code;
}