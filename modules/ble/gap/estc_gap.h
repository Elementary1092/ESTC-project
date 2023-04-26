#ifndef ESTC_BLE_GAP_H_
#define ESTC_BLE_GAP_H_

#include <stdint.h>
#include <app_error.h>

typedef struct
{
	const uint8_t * device_name;
	uint16_t device_name_len;
	uint16_t min_conn_interval_ms;
	uint16_t max_conn_interval_ms;
	uint16_t slave_latency;
	uint16_t conn_supplement_timeout_ms;
} estc_ble_gap_config_t;

ret_code_t estc_ble_gap_peripheral_init(estc_ble_gap_config_t * config);

#endif // ESTC_BLE_GAP_H_