#include <ble_conn_params.h>
#include <string.h>
#include <app_error.h>
#include "estc_gap.h"

ret_code_t estc_ble_gap_peripheral_init(estc_ble_gap_config_t *config)
{
    APP_ERROR_CHECK_BOOL(config != NULL);

    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          config->device_name,
                                          config->device_name_len);
    if (err_code != NRF_SUCCESS) 
	{
		return err_code;
	}

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    if (err_code != NRF_SUCCESS) 
	{
		return err_code;
	}

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MSEC_TO_UNITS(config->min_conn_interval_ms, UNIT_1_25_MS);
    gap_conn_params.max_conn_interval = MSEC_TO_UNITS(config->max_conn_interval_ms, UNIT_1_25_MS);
    gap_conn_params.slave_latency = config->slave_latency;
    gap_conn_params.conn_sup_timeout = MSEC_TO_UNITS(config->conn_supplement_timeout_ms, UNIT_1_25_MS);

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    if (err_code != NRF_SUCCESS) 
	{
		return err_code;
	}

	return NRF_SUCCESS;
}