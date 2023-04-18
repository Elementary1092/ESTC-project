#include "characteristic.h"
#include "modules/ble/ble_helpers.h"

#include <string.h>
#include <app_error.h>

static inline ble_gatt_char_props_t estc_ble_parse_char_props(uint16_t permissions)
{
	return (ble_gatt_char_props_t){
		.broadcast = permissions & ESTC_BLE_CHAR_BROADCAST ? 1 : 0,
		.read = permissions & ESTC_BLE_CHAR_READ ? 1 : 0,
		.write_wo_resp = permissions & ESTC_BLE_CHAR_WRITE_NO_RESP ? 1 : 0,
		.write = permissions & ESTC_BLE_CHAR_WRITE ? 1 : 0,
		.auth_signed_wr = permissions & ESTC_BLE_CHAR_AUTH_WRITE ? 1 : 0,
		.notify = permissions & ESTC_BLE_CHAR_NOTIFICATION ? 1 : 0,
		.indicate = permissions & ESTC_BLE_CHAR_INDICATION ? 1 : 0,
	};
}

static inline ble_gatts_char_pf_t estc_ble_get_fmt(estc_ble_value_type_t value_type)
{
	ble_gatts_char_pf_t presentation_format;
	memset(&presentation_format, 0, sizeof(ble_gatts_char_pf_t));

	switch (value_type)
	{
	case ESTC_BLE_CHAR_TYPE_UINT8:
		presentation_format.format = BLE_GATT_CPF_FORMAT_UINT12;
		break;

	case ESTC_BLE_CHAR_TYPE_UINT16:
		presentation_format.format = BLE_GATT_CPF_FORMAT_UINT16;
		break;

	case ESTC_BLE_CHAR_TYPE_UINT32:
		presentation_format.format = BLE_GATT_CPF_FORMAT_UINT32;
		break;

	case ESTC_BLE_CHAR_TYPE_UINT64:
		presentation_format.format = BLE_GATT_CPF_FORMAT_UINT64;
		break;

	case ESTC_BLE_CHAR_TYPE_INT8:
		presentation_format.format = BLE_GATT_CPF_FORMAT_SINT12;
		break;

	case ESTC_BLE_CHAR_TYPE_INT16:
		presentation_format.format = BLE_GATT_CPF_FORMAT_SINT16;
		break;

	case ESTC_BLE_CHAR_TYPE_INT32:
		presentation_format.format = BLE_GATT_CPF_FORMAT_SINT32;
		break;

	case ESTC_BLE_CHAR_TYPE_INT64:
		presentation_format.format = BLE_GATT_CPF_FORMAT_SINT64;
		break;

	case ESTC_BLE_CHAR_TYPE_FLOAT32:
		presentation_format.format = BLE_GATT_CPF_FORMAT_FLOAT32;
		break;

	case ESTC_BLE_CHAR_TYPE_FLOAT64:
		presentation_format.format = BLE_GATT_CPF_FORMAT_FLOAT64;
		break;

	case ESTC_BLE_CHAR_TYPE_STRING:
		presentation_format.format = BLE_GATT_CPF_FORMAT_UTF8S;
		break;
	}

	return presentation_format;
}

static inline ble_gatt_char_ext_props_t estc_ble_parse_ext_props(uint16_t permissions)
{
	return (ble_gatt_char_ext_props_t){
		.wr_aux = permissions & ESTC_BLE_CHAR_WRITABLE_AUX ? 1 : 0,
		.reliable_wr = permissions & ESTC_BLE_CHAR_RELIABLE_WRITE ? 1 : 0,
	};
}

ret_code_t estc_ble_service_register_characteristic(estc_ble_service_t *service,
													estc_ble_char_cfg_t *config,
													estc_ble_char_handles_t *handles)
{
	APP_ERROR_CHECK_BOOL(service != NULL);
	APP_ERROR_CHECK_BOOL(config != NULL);
	if (config->description_size != 0U)
	{
		APP_ERROR_CHECK_BOOL(config->description_string != NULL);
	}

	ret_code_t error_code = NRF_SUCCESS;

	if (error_code != NRF_SUCCESS)
	{
		return error_code;
	}

	ble_gatts_char_pf_t presentation_format = estc_ble_get_fmt(config->value_type);

	ble_gatts_char_md_t metadata =
		{
			.char_props = estc_ble_parse_char_props(config->permissions),
			.char_ext_props = estc_ble_parse_ext_props(config->permissions),
			.char_user_desc_size = config->description_size,
			.char_user_desc_max_size = config->description_size,
			.p_char_pf = &presentation_format,
			.p_char_user_desc = config->description_string,
		};

	ble_uuid_t characteristic_uuid =
		{
			.type = BLE_UUID_TYPE_BLE,
			.uuid = config->characteristic_uuid,
		};

	ble_gatts_attr_md_t characteristic_metadata = {0};
	characteristic_metadata.vloc = BLE_GATTS_VLOC_USER;
	if (config->value_size_max != 0 && config->value_size != config->value_size_max)
	{
		characteristic_metadata.vlen = 1U;
	}

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&(characteristic_metadata.read_perm));
	if (metadata.char_props.write || metadata.char_props.write_wo_resp)
	{
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&(characteristic_metadata.write_perm));
	}

	ble_gatts_attr_t characteristic =
		{
			.p_attr_md = &characteristic_metadata,
			.p_value = config->value,
			.max_len = config->value_size_max,
			.init_len = config->value_size,
			.p_uuid = &characteristic_uuid,
		};

	ble_gatts_char_handles_t char_handles;

	error_code = sd_ble_gatts_characteristic_add(service->service_handle, &metadata, &characteristic, &char_handles);
	if (error_code != NRF_SUCCESS)
	{
		return error_code;
	}

	handles->value_handle.service_handle = char_handles.value_handle;
	handles->user_description_handle.service_handle = char_handles.user_desc_handle;
	handles->client_char_desc_handle.service_handle = char_handles.cccd_handle;
	handles->server_char_desc_handle.service_handle = char_handles.sccd_handle;

	return error_code;
}

void estc_ble_char_notify(uint16_t conn_handle, uint16_t value_handle, uint8_t *data, uint16_t *data_len)
{
	ble_gatts_hvx_params_t hvx_params =
		{
			.handle = value_handle,
			.type = BLE_GATT_HVX_NOTIFICATION,
			.offset = 0U,
			.p_len = data_len,
			.p_data = data,
		};

	sd_ble_gatts_hvx(conn_handle, &hvx_params);
}

void estc_ble_char_indicate(uint16_t conn_handle, uint16_t value_handle, uint8_t *data, uint16_t *data_len)
{
	ble_gatts_hvx_params_t hvx_params =
		{
			.handle = value_handle,
			.type = BLE_GATT_HVX_INDICATION,
			.offset = 0U,
			.p_len = data_len,
			.p_data = data,
		};

	sd_ble_gatts_hvx(conn_handle, &hvx_params);
}