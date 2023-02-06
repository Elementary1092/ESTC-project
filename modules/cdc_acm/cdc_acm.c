#include <app_timer.h>
#include <nrfx_log.h>
#include "cdc_acm.h"

#define CDC_ACM_TEMP_BUF_SIZE 1

static char resp_buf[CDC_ACM_TEMP_BUF_SIZE] = {0};

void cdc_acm_init(app_usbd_cdc_acm_t const *acm)
{
	app_usbd_class_inst_t const *cdc_acm_class = app_usbd_cdc_acm_class_inst_get(acm);
	APP_ERROR_CHECK(app_usbd_class_append(cdc_acm_class));
}

cdc_acm_ret_code_t cdc_acm_echo(app_usbd_cdc_acm_t const *acm, cdc_acm_read_buf_t *read_buf)
{
	cdc_acm_ret_code_t acm_ret = CDC_ACM_SUCCESS;
	
	cdc_acm_read_buf_append(read_buf, resp_buf[0]);

	if (resp_buf[0] == '\r' \
		|| resp_buf[0] == '\n')
	{
		app_usbd_cdc_acm_write(acm, "\r\n", 2);
		acm_ret = CDC_ACM_READ_NEW_LINE;
	}
	else
	{
		if (resp_buf[0] == ' ' \
			|| resp_buf[0] == '\t')
		{
			acm_ret = CDC_ACM_READ_WHITESPACE;
		}

		app_usbd_cdc_acm_write(acm, resp_buf, CDC_ACM_TEMP_BUF_SIZE);
	}
	
	ret_code_t ret = app_usbd_cdc_acm_read(acm, resp_buf, CDC_ACM_TEMP_BUF_SIZE);
	
	if (ret != NRF_SUCCESS && acm_ret == CDC_ACM_SUCCESS)
	{
		acm_ret = CDC_ACM_ACTION_ERROR;
	}

	return acm_ret;
}

void cdc_acm_only_read(app_usbd_cdc_acm_t const *acm)
{
	(void)app_usbd_cdc_acm_read(acm, resp_buf, CDC_ACM_TEMP_BUF_SIZE);
}

cdc_acm_ret_code_t cdc_acm_write(app_usbd_cdc_acm_t const *acm, const char *str, ssize_t str_len)
{
	ret_code_t err_code = app_usbd_cdc_acm_write(acm, str, str_len);
	if (err_code != NRF_SUCCESS)
	{
		NRF_LOG_ERROR("%s: %s", __func__, NRF_LOG_ERROR_STRING_GET(err_code));
		return CDC_ACM_ACTION_ERROR;
	}

	return CDC_ACM_SUCCESS;
}
