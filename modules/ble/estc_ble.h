#ifndef ESTC_BLE_CONN_PARAMS_H_
#define ESTC_BLE_CONN_PARAMS_H_

#include <stdint.h>
#include <bsp.h>
#include <bsp_btn_ble.h>
#include <ble_conn_params.h>
#include <nrf_ble_qwr.h>

typedef struct
{
	ble_conn_params_evt_handler_t conn_evt_handler;
	ble_srv_error_handler_t conn_error_handler;
	uint32_t first_conn_update_delay;
	uint32_t next_conn_update_delay;
	uint32_t max_conn_update_count;
} estc_ble_conn_peripheral_cfg_t;

typedef void (*estc_ble_connected_subscriber_t)(uint16_t conn_handle);

typedef void (*estc_ble_disconnected_subscriber_t)(uint16_t conn_handle);

ret_code_t estc_ble_stack_init(uint8_t conn_cfg_tag);

ret_code_t estc_ble_conn_peripheral_init(estc_ble_conn_peripheral_cfg_t *config);

void estc_ble_conn_params_err_handler(uint32_t nrf_error);

void estc_ble_conn_params_evt_handler(ble_conn_params_evt_t *evt);

void estc_bsp_default_event_handler(bsp_event_t event);

/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void estc_ble_default_ble_event_handler(ble_evt_t const *p_ble_evt, void *p_context);

void estc_ble_pending_notification(void);

void estc_ble_add_conn_subscriber(estc_ble_connected_subscriber_t sub);

void estc_ble_add_disconn_subscriber(estc_ble_disconnected_subscriber_t sub);

#endif // ESTC_BLE_CONN_PARAMS_H_