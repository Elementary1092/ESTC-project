#include <nrf_log.h>
#include <app_usbd_cdc_acm.h>

#include "cdc_acm_cli.h"

#define CDC_ACM_CLI_HANDLE_EVENT(fns, evt, cdc_acm, arg)                \
	( ( (fns)[ (evt) ] ) ? ( (fns)[ (evt) ] )( (cdc_acm), (arg) ) : 0 ) \

static void cdc_acm_cli_evt_handler(app_usbd_class_inst_t const *p_inst,
									app_usbd_cdc_acm_user_event_t event);

APP_USBD_CDC_ACM_GLOBAL_DEF(cdc_acm_cli,
							cdc_acm_cli_evt_handler,
							CDC_ACM_CLI_COMM_INTERFACE,
							CDC_ACM_CLI_DATA_INTERFACE,
							CDC_ACM_CLI_COMM_EPIN,
							CDC_ACM_CLI_DATA_EPIN,
							CDC_ACM_CLI_DATA_EPOUT,
							APP_USBD_CDC_COMM_PROTOCOL_NONE);

static cdc_acm_cli_event_handler_t handlers[cdc_acm_cli_events_end] = {NULL};

static char word_buf[CDC_ACM_CLI_READ_BUF_SIZE] = {0};

static cdc_acm_read_buf_t acm_read_buffer;

static cdc_acm_ret_code_t cdc_acm_cli_handle_rx_done(void)
{
    cdc_acm_ret_code_t ret;
    do
	{
        ret = cdc_acm_echo(&cdc_acm_cli, &acm_read_buffer);
        if (ret == CDC_ACM_READ_NEW_LINE)
		{
            CDC_ACM_CLI_HANDLE_EVENT(handlers, CDC_ACM_CLI_USB_RX_NEW_LINE, &cdc_acm_cli, &acm_read_buffer);
            cdc_acm_read_buf_clear(&acm_read_buffer);
        }

        CDC_ACM_CLI_HANDLE_EVENT(handlers, CDC_ACM_CLI_USB_RX_DONE, &cdc_acm_cli, &acm_read_buffer);
    }
	while(ret == CDC_ACM_SUCCESS);

    return ret;
}

static void cdc_acm_cli_evt_handler(app_usbd_class_inst_t const *p_inst,
									app_usbd_cdc_acm_user_event_t event)
{
	switch(event)
	{
		case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
		{
			NRF_LOG_INFO("Opened usb port");
			cdc_acm_only_read(&cdc_acm_cli);

			CDC_ACM_CLI_HANDLE_EVENT(handlers, CDC_ACM_CLI_USB_PORT_OPEN, &cdc_acm_cli, &acm_read_buffer);
			break;
		}

		case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
		{
			(void)cdc_acm_cli_handle_rx_done();

			break;
		}

		case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
		{
			CDC_ACM_CLI_HANDLE_EVENT(handlers, CDC_ACM_CLI_USB_TX_DONE, &cdc_acm_cli, &acm_read_buffer);
			break;
		}

		case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
		{
			CDC_ACM_CLI_HANDLE_EVENT(handlers, CDC_ACM_CLI_USB_PORT_CLOSE, &cdc_acm_cli, &acm_read_buffer);
			break;
		}

		default:
			break;
	}
}

void cdc_acm_cli_init(void)
{
	cdc_acm_init(&cdc_acm_cli);
	cdc_acm_read_buf_init(&acm_read_buffer, word_buf, CDC_ACM_CLI_READ_BUF_SIZE);
}

void cdc_acm_add_handler(cdc_acm_cli_event_t evt, cdc_acm_cli_event_handler_t handler)
{
	NRFX_ASSERT(evt < cdc_acm_cli_events_end);

	handlers[evt] = handler;
}