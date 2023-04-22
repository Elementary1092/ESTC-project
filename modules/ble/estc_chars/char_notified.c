#include "char_notified.h"
#include "utils/generator/fcyclic_variable.h"
#include "modules/ble/gatt/estc_gatt_srv_char.h"

APP_TIMER_DEF(m_notification_timer);

static utils_generator_fcyclic_variable_ctx_t notified_value =
{
    .current_value = 0.0F,
    .is_valid = true,
    .max_value = 250.0F,
    .min_value = -250.F,
    .is_increasing = true,
    .step = 5.0F,
};
static estc_ble_srv_char_handles_t notified_handles;

static void ble_notified_value_handler(void *p_ctx)
{
    utils_generator_fcyclic_variable_next(&notified_value);
    uint16_t conn_handle = (uint16_t)((uintptr_t)p_ctx); // to make compiler happy context is first converted to uintptr_t
    uint16_t value_len = sizeof(notified_value.current_value);
    estc_ble_srv_char_notify(
		conn_handle, 
		notified_handles.value_handle.service_handle, 
		(uint8_t *)(&(notified_value.current_value)), 
		&value_len);
}


ret_code_t estc_char_notified_register(estc_ble_service_t * service)
{
	ret_code_t err_code = NRF_SUCCESS;

    err_code = app_timer_create(&m_notification_timer, APP_TIMER_MODE_REPEATED, ble_notified_value_handler);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

    estc_ble_srv_char_cfg_t notified_config =
    {
        .characteristic_uuid = ESTC_NOTIFIED_CHAR_UUID16,
        .permissions = ESTC_BLE_CHAR_READ | ESTC_BLE_CHAR_NOTIFICATION,
        .value = (uint8_t *)(&notified_value.current_value),
        .value_size = sizeof(notified_value.current_value),
        .value_type = ESTC_BLE_CHAR_TYPE_FLOAT32,
        .value_size_max = sizeof(notified_value.current_value),
        .characteristic_uuid_type = BLE_UUID_TYPE_BLE,
    };

    err_code = estc_ble_srv_char_register(service, &notified_config, &notified_handles);

	return err_code;
}

void estc_char_notified_start(uint16_t conn_handle)
{
	app_timer_start(m_notification_timer, ESTC_NOTIFIED_VALUE_DELAY, (void *)((uintptr_t )conn_handle));
}

void estc_char_notified_stop(uint16_t conn_handle)
{
	app_timer_stop(m_notification_timer);
}
