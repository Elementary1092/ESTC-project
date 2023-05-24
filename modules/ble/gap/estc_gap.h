#ifndef ESTC_BLE_GAP_H_
#define ESTC_BLE_GAP_H_

#include <stdint.h>
#include <app_error.h>

/**
 * @brief Configuration of GAP module when device is initialized as peripheral.
*/
typedef struct
{
	const uint8_t * device_name;
	uint16_t device_name_len;
	uint16_t min_conn_interval_ms;
	uint16_t max_conn_interval_ms;
	uint16_t slave_latency;
	uint16_t conn_supplement_timeout_ms;
} estc_ble_gap_config_t;

/**
 * @brief Initialize GAP module.
 * 
 * @param[in] config GAP configuration parameters.
 * 
 * @retval NRF_ERROR_DATA_SIZE - invalid length of the device name (should be less than 31 B).
 * 
 * @retval NRF_ERROR_FORBIDDEN - invalid name of the device.
 * 
 * @retval NRF_SUCCESS - successfully initialized GAP module.
*/
ret_code_t estc_ble_gap_peripheral_init(estc_ble_gap_config_t * config);

#endif // ESTC_BLE_GAP_H_