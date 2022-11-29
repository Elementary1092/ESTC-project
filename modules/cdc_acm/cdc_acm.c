#include <app_timer.h>
#include <nrfx_log.h>
#include "cdc_acm.h"

#define CDC_ACM_TEMP_BUF_SIZE 1

#define EVENT_QUEUE_PROCESS_TIMEOUT APP_TIMER_TICKS(50)

APP_TIMER_DEF(event_queue_process_timer);

static void event_queue_process_handler(void *p_ctx)
{
	(void)app_usbd_event_queue_process();
}

void cdc_acm_init(app_usbd_cdc_acm_t const *acm)
{
	app_usbd_class_inst_t const *cdc_acm_class = app_usbd_cdc_acm_class_inst_get(acm);
	APP_ERROR_CHECK(app_usbd_class_append(cdc_acm_class));

	app_timer_create(&event_queue_process_timer, APP_TIMER_MODE_REPEATED, event_queue_process_handler);

	app_timer_start(event_queue_process_timer, EVENT_QUEUE_PROCESS_TIMEOUT, NULL);
}

ssize_t cdc_acm_echo(app_usbd_cdc_acm_t const *acm, char *buf, ssize_t buf_size)
{
	char temp_buf[CDC_ACM_TEMP_BUF_SIZE] = {0};
	ret_code_t ret;
	ssize_t read_chars_count = 0;

	for (ssize_t i = 0; i < buf_size; i++)
	{
		ret = app_usbd_cdc_acm_read(acm, temp_buf, CDC_ACM_TEMP_BUF_SIZE);

		if (ret != NRF_SUCCESS)
		{
#if NRF_LOG_ENABLED
			NRF_LOG_ERROR("cdc_acm_echo: On reading from cdc acm: %u", ret);
#endif
			continue;
		}

		if (temp_buf[0] == '\r' || temp_buf[0] == '\n')
		{
			(void)app_usbd_cdc_acm_write(acm, "\r\n", 2);
			return read_chars_count;
		}
		else
		{
			(void)app_usbd_cdc_acm_write(acm, temp_buf, CDC_ACM_TEMP_BUF_SIZE);
		}

		if (temp_buf[0] == ' ' || temp_buf[0] == '\t')
		{
			return read_chars_count;
		}

		buf[i] = temp_buf[0];
		read_chars_count++;
	}

	return read_chars_count;
}

ssize_t cdc_acm_read(app_usbd_cdc_acm_t const *acm, char *buf, ssize_t buf_size)
{
	char temp_buf[CDC_ACM_TEMP_BUF_SIZE] = {0};
	ret_code_t ret;
	ssize_t read_chars_count = 0;

	for (ssize_t i = 0; i < buf_size; i++)
	{
		ret = app_usbd_cdc_acm_read(acm, temp_buf, CDC_ACM_TEMP_BUF_SIZE);

		if (ret != NRF_SUCCESS)
		{
#if NRF_LOG_ENABLED
			NRF_LOG_ERROR("cdc_acm_read: On reading from cdc acm: %u", ret);
#endif
			continue;
		}

		if (temp_buf[0] == '\r' \
			|| temp_buf[0] == '\n' \
			|| temp_buf[0] == ' ' \
			|| temp_buf[0] == '\t')
		{
			return read_chars_count;
		}

		buf[i] = temp_buf[0];
		read_chars_count++;
	}

	return read_chars_count;
}

void cdc_acm_write(app_usbd_cdc_acm_t const *acm, const char *str, ssize_t str_len)
{
	(void)app_usbd_cdc_acm_write(acm, str, str_len);
}
