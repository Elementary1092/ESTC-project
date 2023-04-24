#include <app_error.h>
#include <nrf_ble_bms.h>
#include <ble_conn_state.h>
#include <nrf_ble_qwr.h>
#include <peer_manager.h>
#include <peer_manager_handler.h>
#include <nrf_log.h>
#include "estc_bond.h"

NRF_BLE_QWR_DEF(qwr_bond);
NRF_BLE_BMS_DEF(m_bms);

static ble_conn_state_user_flag_id_t m_bms_bonds_to_delete;

static uint8_t m_auth_code[] = {'A', 'B', 'C', 'D'}; //0x41, 0x42, 0x43, 0x44
static int m_auth_code_len = sizeof(m_auth_code);

static uint8_t m_qwr_mem[ESTC_BLE_GAP_BOND_MEM_BUFF_SIZE];

static void bms_evt_handler(nrf_ble_bms_t *p_ess, nrf_ble_bms_evt_t *p_evt)
{
	ret_code_t err_code;
	bool is_authorized = true;

	switch (p_evt->evt_type)
	{
	case NRF_BLE_BMS_EVT_AUTH:
		NRF_LOG_INFO("Authorization request.");
		if ((p_evt->auth_code.len != m_auth_code_len) ||
			(memcmp(m_auth_code, p_evt->auth_code.code, m_auth_code_len) != 0))
		{
			is_authorized = false;
		}

		err_code = nrf_ble_bms_auth_response(&m_bms, is_authorized);
		APP_ERROR_CHECK(err_code);
	}
}

static void service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void delete_requesting_bond(nrf_ble_bms_t const * p_bms)
{
    NRF_LOG_INFO("Client requested that bond to current device deleted");
    ble_conn_state_user_flag_set(p_bms->conn_handle, m_bms_bonds_to_delete, true);
}

static void bond_delete(uint16_t conn_handle, void * p_context)
{
    UNUSED_PARAMETER(p_context);
    ret_code_t   err_code;
    pm_peer_id_t peer_id;

    if (ble_conn_state_status(conn_handle) == BLE_CONN_STATUS_CONNECTED)
    {
        ble_conn_state_user_flag_set(conn_handle, m_bms_bonds_to_delete, true);
    }
    else
    {
        NRF_LOG_DEBUG("Attempting to delete bond.");
        err_code = pm_peer_id_get(conn_handle, &peer_id);
        APP_ERROR_CHECK(err_code);
        if (peer_id != PM_PEER_ID_INVALID)
        {
            err_code = pm_peer_delete(peer_id);
            APP_ERROR_CHECK(err_code);
            ble_conn_state_user_flag_set(conn_handle, m_bms_bonds_to_delete, false);
        }
    }
}

/**
 * @brief Function for deleting all bonds
*/
static void delete_all_bonds(nrf_ble_bms_t const * p_bms)
{
    ret_code_t err_code;
    uint16_t conn_handle;

    NRF_LOG_INFO("Client requested that all bonds be deleted");

    pm_peer_id_t peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
    while (peer_id != PM_PEER_ID_INVALID)
    {
        err_code = pm_conn_handle_get(peer_id, &conn_handle);
        APP_ERROR_CHECK(err_code);

        bond_delete(conn_handle, NULL);

        peer_id = pm_next_peer_id_get(peer_id);
    }
}


/**@brief Function for deleting all bet requesting device bonds
*/
static void delete_all_except_requesting_bond(nrf_ble_bms_t const * p_bms)
{
    ret_code_t err_code;
    uint16_t conn_handle;

    NRF_LOG_INFO("Client requested that all bonds except current bond be deleted");

    pm_peer_id_t peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
    while (peer_id != PM_PEER_ID_INVALID)
    {
        err_code = pm_conn_handle_get(peer_id, &conn_handle);
        APP_ERROR_CHECK(err_code);

        /* Do nothing if this is our own bond. */
        if (conn_handle != p_bms->conn_handle)
        {
            bond_delete(conn_handle, NULL);
        }

        peer_id = pm_next_peer_id_get(peer_id);
    }
}

static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static uint16_t qwr_evt_handler(nrf_ble_qwr_t * p_qwr, nrf_ble_qwr_evt_t * p_evt)
{
    return nrf_ble_bms_on_qwr_evt(&m_bms, p_qwr, p_evt);
}

static void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_disconnect_on_sec_failure(p_evt);
    pm_handler_flash_clean(p_evt);
}

void estc_ble_gap_bond_init(void)
{
	ret_code_t err_code;
	nrf_ble_bms_init_t bms_init;
	nrf_ble_qwr_init_t   qwr_init;

    // Initialize Queued Write Module
    memset(&qwr_init, 0, sizeof(qwr_init));
    qwr_init.mem_buffer.len   = ESTC_BLE_GAP_BOND_MEM_BUFF_SIZE;
    qwr_init.mem_buffer.p_mem = m_qwr_mem;
    qwr_init.callback         = qwr_evt_handler;
    qwr_init.error_handler    = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&qwr_bond, &qwr_init);
    APP_ERROR_CHECK(err_code);

	// Initialize Bond Management Service
    memset(&bms_init, 0, sizeof(bms_init));

    m_bms_bonds_to_delete        = ble_conn_state_user_flag_acquire();
    bms_init.evt_handler         = bms_evt_handler;
    bms_init.error_handler       = service_error_handler;
    bms_init.feature.delete_requesting_auth         = true;
    bms_init.feature.delete_all_auth                = true;
    bms_init.feature.delete_all_but_requesting_auth = true;
    bms_init.bms_feature_sec_req = SEC_JUST_WORKS;
    bms_init.bms_ctrlpt_sec_req  = SEC_JUST_WORKS;

    bms_init.p_qwr                                       = &qwr_bond;
    bms_init.bond_callbacks.delete_requesting            = delete_requesting_bond;
    bms_init.bond_callbacks.delete_all                   = delete_all_bonds;
    bms_init.bond_callbacks.delete_all_except_requesting = delete_all_except_requesting_bond;

    err_code = nrf_ble_bms_init(&m_bms, &bms_init);
    APP_ERROR_CHECK(err_code);

    ble_gap_sec_params_t sec_param;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}
