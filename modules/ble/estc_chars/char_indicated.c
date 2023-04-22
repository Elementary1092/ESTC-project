#include <nrf_log.h>
#include "modules/ble/gatt/estc_gatt_srv_char.h"
#include "utils/generator/sinusoid.h"
#include "char_indicated.h"

APP_TIMER_DEF(m_indication_timer);

static utils_generator_sinusoid_ctx_t indicated_value =
{
    .angle_factor = 2.0F,
    .angle_step = 1.0F,
    .current_angle = 0.0F,
    .max_angle_value = 250.0F,
    .current_value = 0.0F,
};
static estc_ble_srv_char_handles_t indicated_handles;

static void ble_indicated_value_handler(void *p_ctx)
{
    utils_generator_sinusoid_next(&indicated_value);
    uint16_t conn_handle = (uint16_t)((uintptr_t)p_ctx); // to make compiler happy context is first converted to uintptr_t
    NRF_LOG_INFO("Conn handle: %d", conn_handle);
    uint16_t value_len = sizeof(indicated_value.current_value);
    estc_ble_srv_char_indicate(
		conn_handle, 
		indicated_handles.value_handle.service_handle, 
		(uint8_t *)(&(indicated_value.current_value)), 
		&value_len);
}

ret_code_t estc_char_indicated_register(estc_ble_service_t * service)
{
	ret_code_t err_code = NRF_SUCCESS;

    err_code = app_timer_create(&m_indication_timer, APP_TIMER_MODE_REPEATED, ble_indicated_value_handler);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

    estc_ble_srv_char_cfg_t indicated_config =
    {
        .characteristic_uuid = ESTC_INDICATED_CHAR_UUID16,
        .permissions = ESTC_BLE_CHAR_READ | ESTC_BLE_CHAR_INDICATION,
        .value = (uint8_t *)(&indicated_value.current_value),
        .value_size = sizeof(indicated_value.current_value),
        .value_type = ESTC_BLE_CHAR_TYPE_FLOAT32,
        .value_size_max = sizeof(indicated_value.current_value),
        .characteristic_uuid_type = BLE_UUID_TYPE_BLE,
    };

    err_code = estc_ble_srv_char_register(service, &indicated_config, &indicated_handles);
	return err_code;
}

void estc_char_indicated_start(uint16_t conn_handle)
{
    app_timer_start(m_indication_timer, ESTC_INDICATED_VALUE_DELAY, (void *)((uintptr_t )conn_handle));
}

void estc_char_indicated_stop(uint16_t conn_handle)
{
	app_timer_stop(m_indication_timer);
}
