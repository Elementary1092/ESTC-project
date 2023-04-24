#include <app_error.h>
#include "estc_ble_write_mngr.h"

static uint16_t sub_value_handles[ESTC_BLE_WRITE_MNGR_MAX_SUBS];
static estc_ble_write_mngr_subscriber_t sub_handlers[ESTC_BLE_WRITE_MNGR_MAX_SUBS];
static uint8_t subs_end_idx;

ret_code_t estc_ble_write_mngr_subscribe(uint16_t value_handle, estc_ble_write_mngr_subscriber_t handler)
{
	if (subs_end_idx >= ESTC_BLE_WRITE_MNGR_MAX_SUBS)
	{
		return NRF_ERROR_STORAGE_FULL;
	}

	sub_value_handles[subs_end_idx] = value_handle;
	sub_handlers[subs_end_idx] = handler;
	subs_end_idx++;

	return NRF_SUCCESS;
}

void estc_ble_write_mngr_handle(uint16_t conn_handle, uint16_t value_handle, uint8_t * data, uint16_t offset, uint16_t len)
{
	for (uint8_t i = 0U; i < subs_end_idx; i++)
	{
		if (value_handle == sub_value_handles[i])
		{
			sub_handlers[i](conn_handle, data, offset, len);
			return;
		}
	}
}
