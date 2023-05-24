#ifndef ESTC_BLE_CONN_PARAMS_H_
#define ESTC_BLE_CONN_PARAMS_H_

#include <stdint.h>
#include <bsp.h>
#include <bsp_btn_ble.h>
#include <ble_conn_params.h>
#include <nrf_ble_qwr.h>

/**
 * @brief Configuration of the peripheral connection preferences. 
*/
typedef struct
{
	ble_conn_params_evt_handler_t conn_evt_handler;
	ble_srv_error_handler_t conn_error_handler;
	uint32_t first_conn_update_delay;
	uint32_t next_conn_update_delay;
	uint32_t max_conn_update_count;
} estc_ble_conn_peripheral_cfg_t;

/**
 * @brief Callback signature which should be called on ble connection event.
*/
typedef void (*estc_ble_connected_subscriber_t)(uint16_t conn_handle);

/**
 * @brief Callback signature which should be called on ble disconnection event.
*/
typedef void (*estc_ble_disconnected_subscriber_t)(uint16_t conn_handle);

/**
 * @brief Initialize ble stack and enable softdevice.
 * 
 * @param[in] conn_cfg_tag - connection which should be configured.
 * 
 * @retval NRF_ERROR_INVALID_STATE - softdevice is already enabled.
 * 
 * @retval NRF_ERROR_NO_MEM - not enough memory.
 * 
 * @retval NRF_SUCCESS - successfully initialized ble stack
*/
ret_code_t estc_ble_stack_init(uint8_t conn_cfg_tag);

/**
 * @brief Set ble peripheral role connection preferences.
 * 
 * @param[in] config desired configuration of connection preferences.
 * 
 * @retval NRF_ERROR_INVALID_STATE - failed to start timer.
 * 
 * @retval NRF_SUCCESS - successfully set connection preferences. 
*/
ret_code_t estc_ble_conn_peripheral_init(estc_ble_conn_peripheral_cfg_t *config);

/**
 * @brief Default ble connection error handler. Resets device on error.
*/
void estc_ble_conn_params_err_handler(uint32_t nrf_error);

/**
 * @brief Default ble connection event handler. Termincates connection invalid connection parameters.
*/
void estc_ble_conn_params_evt_handler(ble_conn_params_evt_t *evt);

/**
 * @brief Default bsp event handler. Terminates connection on BLE_DISCONNECTED event.
*/
void estc_bsp_default_event_handler(bsp_event_t event);

/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void estc_ble_default_ble_event_handler(ble_evt_t const *p_ble_evt, void *p_context);

/**
 * @brief Indicate that new notification is to be send.
*/
void estc_ble_pending_notification(void);

/**
 * @brief Set callback which should be called on ble connection event.
*/
void estc_ble_add_conn_subscriber(estc_ble_connected_subscriber_t sub);

/**
 * @brief Set callback which should be called on ble disconnection event.
*/
void estc_ble_add_disconn_subscriber(estc_ble_disconnected_subscriber_t sub);

#endif // ESTC_BLE_CONN_PARAMS_H_