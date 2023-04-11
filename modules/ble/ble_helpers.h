#ifndef ESTC_BLE_HELPERS_H_
#define ESTC_BLE_HELPERS_H_

#include <ble.h>
#include <ble_conn_params.h>
#include  <bsp.h>

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3						 /**< Number of attempts before giving up the connection parameter negotiation. */

/***
 * @brief Initializes nrf_ble_qwr instance
 */
void estc_ble_qwr_init(void);

uint16_t estc_ble_get_conn_handle(void);

/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void estc_ble_default_ble_event_handler(ble_evt_t const *p_ble_evt, void *p_context);

/**
 * @brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
void estc_bsp_default_event_handler(bsp_event_t event);

/**
 * @brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
void estc_ble_conn_params_init(void);

#endif // ESTC_BLE_HELPERS_H_