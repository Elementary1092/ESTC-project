#include <app_error.h>
#include <nrf_sdh_ble.h>
#include <stdbool.h>
#include "estc_ble_qwr.h"

NRF_BLE_QWR_DEF(m_qwr); /**< Context for the Queued Write module.*/

static bool is_inited = false;

static uint8_t m_qwr_mem[ESTC_BLE_QWR_MEM_BUFF_SIZE];

/**
 * @brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}

void estc_ble_qwr_init(estc_ble_qwr_evt_handler_t evt_handler)
{
	APP_ERROR_CHECK_BOOL(evt_handler != NULL);
	
	ret_code_t err_code = NRF_SUCCESS;
	nrf_ble_qwr_init_t qwr_init = {0};

	// Initialize Queued Write Module.    
	qwr_init.mem_buffer.len   = ESTC_BLE_QWR_MEM_BUFF_SIZE;
	qwr_init.mem_buffer.p_mem = m_qwr_mem;
	qwr_init.callback = (nrf_ble_qwr_evt_handler_t)evt_handler;
	qwr_init.error_handler = nrf_qwr_error_handler;

	err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
	APP_ERROR_CHECK(err_code);

	is_inited = true;
}

nrf_ble_qwr_t * estc_ble_qwr_get(void)
{
	if (!is_inited)
	{
		return NULL;
	}

	return &m_qwr;
}
