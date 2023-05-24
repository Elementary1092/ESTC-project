#include <string.h>
#include <app_error.h>
#include <nrf_ble_qwr.h>
#include <bsp_btn_ble.h>
#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <nrf_ble_gatt.h>
#include <nrf_log.h>
#include <app_timer.h>

#include "estc_ble.h"
#include "estc_ble_write_mngr.h"
#include "modules/ble/gap/bond/estc_bond.h"
#include "modules/ble/estc_ble_qwr.h"

#define ESTC_MAX_CONN_SUBSCRIBERS 5

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

static uint32_t notifications_to_send = 0U;

static estc_ble_connected_subscriber_t conn_subscribers[ESTC_MAX_CONN_SUBSCRIBERS];

static uint8_t conn_sub_idx = 0U;

static estc_ble_disconnected_subscriber_t disconn_subscribers[ESTC_MAX_CONN_SUBSCRIBERS];

static uint8_t disconn_sub_idx = 0U;

ret_code_t estc_ble_stack_init(uint8_t conn_cfg_tag)
{
	ret_code_t err_code = NRF_SUCCESS;

	err_code = nrf_sdh_enable_request();
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

	// Configure the BLE stack using the default settings.
	// Fetch the start address of the application RAM.
	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(conn_cfg_tag, &ram_start);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

	// Enable BLE stack.
	err_code = nrf_sdh_ble_enable(&ram_start);

	return err_code;
}

ret_code_t estc_ble_conn_peripheral_init(estc_ble_conn_peripheral_cfg_t *config)
{
	ret_code_t error_code = NRF_SUCCESS;
	ble_conn_params_init_t cp_init;

	memset(&cp_init, 0, sizeof(cp_init));

	cp_init.p_conn_params = NULL; // get conn params from the host
	cp_init.disconnect_on_fail = false;
	cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
	cp_init.first_conn_params_update_delay = config->first_conn_update_delay;
	cp_init.next_conn_params_update_delay = config->next_conn_update_delay;
	cp_init.max_conn_params_update_count = config->max_conn_update_count;
	cp_init.evt_handler = config->conn_evt_handler;
	cp_init.error_handler = config->conn_error_handler;

	error_code = ble_conn_params_init(&cp_init);

	return error_code;
}

void estc_ble_conn_params_err_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}

void estc_ble_conn_params_evt_handler(ble_conn_params_evt_t *p_evt)
{
	ret_code_t err_code;

	if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
	{
		err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
		NRF_LOG_INFO("Got error on connection: ", NRF_LOG_ERROR_STRING_GET(err_code));
		APP_ERROR_CHECK(err_code);
	}
}

void estc_bsp_default_event_handler(bsp_event_t event)
{
	ret_code_t err_code;

	switch (event)
	{
	case BSP_EVENT_DISCONNECT:
		err_code = sd_ble_gap_disconnect(m_conn_handle,
										 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		if (err_code != NRF_ERROR_INVALID_STATE)
		{
			APP_ERROR_CHECK(err_code);
		}
		break; // BSP_EVENT_DISCONNECT
	default:
		break;
	}
}

void estc_ble_default_ble_event_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
	ret_code_t err_code = NRF_SUCCESS;

	switch (p_ble_evt->header.evt_id)
	{
	case BLE_GAP_EVT_DISCONNECTED:
		NRF_LOG_INFO("Disconnected.");
		for (uint8_t i = 0U; i < disconn_sub_idx; i++)
		{
			if (disconn_subscribers[i] == NULL)
			{
				break;
			}

			disconn_subscribers[i](m_conn_handle);
		}
		notifications_to_send = 0U;
		estc_ble_gap_bond_delete_unused();
		m_conn_handle = BLE_CONN_HANDLE_INVALID;
		break;

	case BLE_GAP_EVT_CONNECTED:
		NRF_LOG_INFO("Connected.");
		APP_ERROR_CHECK(err_code);
		m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
		err_code = nrf_ble_qwr_conn_handle_assign(estc_ble_qwr_get(), p_ble_evt->evt.gap_evt.conn_handle);
		APP_ERROR_CHECK(err_code);
		for (uint8_t i = 0U; i < conn_sub_idx; i++)
		{
			if (conn_subscribers[i] == NULL)
			{
				break;
			}

			conn_subscribers[i](m_conn_handle);
		}
		break;

	case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
	{
		NRF_LOG_DEBUG("PHY update request.");
		ble_gap_phys_t const phys =
			{
				.rx_phys = BLE_GAP_PHY_AUTO,
				.tx_phys = BLE_GAP_PHY_AUTO,
			};
		err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
		APP_ERROR_CHECK(err_code);
	}
	break;

	case BLE_GATTC_EVT_TIMEOUT:
		// Disconnect on GATT Client timeout event.
		NRF_LOG_DEBUG("GATT Client Timeout.");
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
										 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_GATTS_EVT_TIMEOUT:
		// Disconnect on GATT Server timeout event.
		NRF_LOG_DEBUG("GATT Server Timeout.");
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
										 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		APP_ERROR_CHECK(err_code);
		notifications_to_send--;
		break;

	case BLE_GATTS_EVT_HVN_TX_COMPLETE:
		notifications_to_send -= (uint32_t)p_ble_evt->evt.gatts_evt.params.hvn_tx_complete.count;
		NRF_LOG_INFO("Sent notification to %hu successfully. Notifications sent: %hhu. Left to send: %u",
					  p_ble_evt->evt.gatts_evt.conn_handle,
					  p_ble_evt->evt.gatts_evt.params.hvn_tx_complete.count, 
					  notifications_to_send);
		break;

	case BLE_GATTS_EVT_WRITE:
		{
			NRF_LOG_INFO("Received write request.");
			ble_gatts_evt_write_t write_evt = p_ble_evt->evt.gatts_evt.params.write;
			estc_ble_write_mngr_handle(
				p_ble_evt->evt.gatts_evt.conn_handle, 
				write_evt.handle, 
				write_evt.data, 
				write_evt.offset, 
				write_evt.len);
		}
		break;

	default:
		// No implementation needed.
		break;
	}
}

void estc_ble_pending_notification(void)
{
	notifications_to_send++;
}

void estc_ble_add_conn_subscriber(estc_ble_connected_subscriber_t sub)
{
	if (sub == NULL || conn_sub_idx >= ESTC_MAX_CONN_SUBSCRIBERS)
	{
		return;
	}

	conn_subscribers[conn_sub_idx++] = sub;
}

void estc_ble_add_disconn_subscriber(estc_ble_disconnected_subscriber_t sub)
{
	if (sub == NULL || disconn_sub_idx >= ESTC_MAX_CONN_SUBSCRIBERS)
	{
		return;
	}

	disconn_subscribers[disconn_sub_idx++] = sub;
}
