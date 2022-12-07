#ifndef HSV_CLI_H
#define HSV_CLI_H

#include <stdint.h>
#include <stdbool.h>
#include "../cdc_acm/cdc_acm_read_buf.h"

/*
	hsv_cli_exec_command parses { read_buf }, 
	resolves specified command (first word from the { read_buf } is treated as a command),
	and executes this command (all words except the first word from the { read_buf } are treated as arguments of the command). 
*/
void hsv_cli_exec_command(app_usbd_cdc_acm_t const *cdc_acm, cdc_acm_read_buf_ctx_t *read_buf);

#endif // HSV_CLI_H