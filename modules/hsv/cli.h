#ifndef HSV_CLI_H
#define HSV_CLI_H

#include <stdint.h>
#include <stdbool.h>
#include "../cdc_acm/cdc_acm_read_buf.h"

void hsv_cli_exec_command(app_usbd_cdc_acm_t const *cdc_acm, cdc_acm_read_buf_ctx_t *read_buf);

#endif // HSV_CLI_H