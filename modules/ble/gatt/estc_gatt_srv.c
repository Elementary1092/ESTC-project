#include "modules/ble/gatt/estc_gatt_srv.h"

#include <string.h>

#include <app_error.h>
#include <nrf_log.h>
#include <ble.h>
#include <ble_gatts.h>
#include <ble_srv_common.h>

static uint8_t ser_uuids[ESTC_BLE_MAX_SERVICES][16];
static uint8_t ser_uuid_idx;

ret_code_t estc_ble_service_init(estc_ble_service_t *service, uint8_t service_uuid128[16], uint16_t service_id)
{
	APP_ERROR_CHECK_BOOL(service != NULL);
	APP_ERROR_CHECK_BOOL(service_uuid128 != NULL);

	if (ser_uuid_idx >= ESTC_BLE_MAX_SERVICES)
	{
		return NRF_ERROR_NO_MEM;
	}

	ret_code_t error_code = NRF_SUCCESS;
	
	ble_uuid128_t p_vs_uuid;
	memcpy(p_vs_uuid.uuid128, service_uuid128, sizeof(p_vs_uuid.uuid128));

	service->uuid_type = BLE_UUID_TYPE_UNKNOWN;
	error_code = sd_ble_uuid_vs_add(&p_vs_uuid, &(service->uuid_type));
	if (error_code != NRF_SUCCESS)
	{
		return error_code;
	}
	memcpy(ser_uuids[ser_uuid_idx], service_uuid128, sizeof(ser_uuids[ser_uuid_idx]));
	service->uuid128 = ser_uuids[ser_uuid_idx];

	ble_uuid_t service_uuid =
	{
		.uuid = service_id,
		.type = BLE_UUID_TYPE_VENDOR_BEGIN,
	};

	error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &service->service_handle);

	return error_code;
}