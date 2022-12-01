#ifndef HSV_CLI_H
#define HSV_CLI_H

#include <stdint.h>
#include <stdbool.h>
#include "../cdc_acm/cdc_acm_read_buf.h"

#define HSV_CLI_MAX_ARGS_SIZE     3
#define HSV_CLI_COMMAND_HELP_STR "help"
#define HSV_CLI_COMMAND_HSV_STR  "hsv"
#define HSV_CLI_COMMAND_RGB_STR  "rgb"

typedef enum
{
	HSV_CLI_COMMAND_UNKNOWN,
	HSV_CLI_COMMAND_HELP,
	HSV_CLI_COMMAND_UPDATE_HSV,
	HSV_CLI_COMMAND_UPDATE_RGB
} hsv_cli_command_t;

void hsv_cli_exec_command(app_usbd_cdc_acm_t const *cdc_acm, cdc_acm_read_buf_ctx_t *read_buf);

#endif // HSV_CLI_H