#include <nrf_log.h>
#include <app_usbd_cdc_acm.h>
#include <string.h>
#include <stdlib.h>

#include "cli.h"
#include "hsv_picker.h"
#include "cli_handlers.h"
#include "utils/strings/strings.h"
#include "modules/cdc_acm/cdc_acm.h"
#include "app_config.h"

#define HSV_CLI_MAX_ARGS      5

#ifndef HSV_CLI_MAX_WORD_SIZE
#define HSV_CLI_MAX_WORD_SIZE ESTC_MAX_LINE_SIZE
#endif

estc_cli_described_result_t hsv_cli_exec_command(cdc_acm_read_buf_t *read_buf)
{
	char args[HSV_CLI_MAX_ARGS + 1][HSV_CLI_MAX_WORD_SIZE] = {0};
	size_t nargs = utils_strings_tokenize(args, read_buf->buf, " \r\n\t\0");

	if (nargs == 0)
	{
		return (estc_cli_described_result_t) {
			.error = ESTC_CLI_ERROR_EMPTY_COMMAND,
			.result_describer = estc_cli_describe_error,
		};
	}
	
	hsv_cli_command_t cmd = hsv_cli_resolve_command(args[0]);
	
	return hsv_cli_exec_handler(cmd, args + 1, nargs - 1);
}
