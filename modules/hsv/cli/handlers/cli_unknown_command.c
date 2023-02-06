#include "include/cli_unknown_command.h"
#include "modules/cdc_acm/cdc_acm.h"

const char *hsv_cli_handler_unknown_command_name(void)
{
	return "";
}

const char *hsv_cli_handler_unknown_command_help(void)
{
	return "";
}

void hsv_cli_handler_unknown(app_usbd_cdc_acm_t const *cdc_acm, 
                             char args[][HSV_CLI_MAX_WORD_SIZE], 
							 uint8_t nargs)
{
	const char *message = "Unknown command. Try again.\r\n";
	cdc_acm_write(cdc_acm, message, strlen(message));
}

hsv_cli_handler_i hsv_cli_unknown_command(void)
{
	return (hsv_cli_handler_i) {
		.command_name = hsv_cli_handler_unknown_command_name,
		.help = hsv_cli_handler_unknown_command_help,
		.command_handler = hsv_cli_handler_unknown,
	};
}