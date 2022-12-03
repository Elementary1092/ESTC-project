#ifndef CDC_ACM_CLI_H
#define CDC_ACM_CLI_H

#include "cdc_acm.h"
#include "app_config.h"

#ifndef CDC_ACM_CLI_COMM_EPIN
#define CDC_ACM_CLI_COMM_EPIN NRFX_USBD_EPIN3
#endif

#ifndef CDC_ACM_CLI_COMM_INTERFACE
#define CDC_ACM_CLI_COMM_INTERFACE 2
#endif

#ifndef CDC_ACM_CLI_DATA_INTERFACE
#define CDC_ACM_CLI_DATA_INTERFACE 3
#endif

#ifndef CDC_ACM_CLI_DATA_EPIN
#define CDC_ACM_CLI_DATA_EPIN NRFX_USBD_EPIN4
#endif

#ifndef CDC_ACM_CLI_DATA_EPOUT
#define CDC_ACM_CLI_DATA_EPOUT NRFX_USBD_EPOUT4
#endif

#ifndef CDC_ACM_CLI_READ_BUF_SIZE
#define CDC_ACM_CLI_READ_BUF_SIZE ESTC_MAX_LINE_SIZE
#endif

typedef enum
{
	CDC_ACM_CLI_USB_PORT_OPEN,
	CDC_ACM_CLI_USB_RX_DONE,
	CDC_ACM_CLI_USB_RX_NEW_LINE,
	CDC_ACM_CLI_USB_TX_DONE,
	CDC_ACM_CLI_USB_PORT_CLOSE,

	cdc_acm_cli_events_end // should not be used anywhere
} cdc_acm_cli_event_t;

typedef void (*cdc_acm_cli_event_handler_t)(app_usbd_cdc_acm_t const *cdc_acm, cdc_acm_read_buf_ctx_t *read_buf);

void cdc_acm_cli_init(void);

void cdc_acm_add_handler(cdc_acm_cli_event_t evt, cdc_acm_cli_event_handler_t handler);

#endif