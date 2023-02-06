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

/**
 * @brief Possible events which are emitted by this module.
*/
typedef enum
{
	CDC_ACM_CLI_USB_PORT_OPEN,
	CDC_ACM_CLI_USB_RX_DONE,
	CDC_ACM_CLI_USB_RX_NEW_LINE,
	CDC_ACM_CLI_USB_TX_DONE,
	CDC_ACM_CLI_USB_PORT_CLOSE,

	cdc_acm_cli_events_end // should not be used anywhere
} cdc_acm_cli_event_t;

/**
 * @brief The type of a function which may handle events emitted by this module.
 * 
 * @param[in] cdc_acm Handler will get registered cdc acm instance and may write to it.
 * 
 * @param[in] read_buf Input which may be processed by the handler.
*/
typedef void (*cdc_acm_cli_event_handler_t)(app_usbd_cdc_acm_t const *cdc_acm, cdc_acm_read_buf_t *read_buf);

/**
 * @brief Register cdc acm instance.
*/
void cdc_acm_cli_init(void);

/**
 * @brief Registers a handler which should be triggered on cdc_acm_cli_event_t occurence.
 * 
 * @param[in] evt The type of an event when the handler should be triggered.
 * 
 * @param[in] handler The handler which should process an event.
*/
void cdc_acm_add_handler(cdc_acm_cli_event_t evt, cdc_acm_cli_event_handler_t handler);

#endif