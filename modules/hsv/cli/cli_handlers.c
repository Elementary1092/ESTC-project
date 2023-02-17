#include <nrfx_log.h>
#include <string.h>

#include "utils/strings/strings.h"
#include "utils/hash/hash.h"
#include "modules/cdc_acm/cdc_acm.h"
#include "cli_handlers.h"
#include "hsv_picker.h"
#include "hsv_saved_colors.h"

static hsv_cli_command_handler_get command_handlers[] =
{
	HSV_CLI_HANDLERS(HSV_CLI_EXECUTORS_EXPAND_AS_INTERFACE)
};

static const char *hsv_cli_get_cmd_str(hsv_cli_command_t h)
{
	if (h >= HSV_CLI_COMMAND_HELP)
	{
		return "";
	}

	hsv_cli_handler_i handler = (*command_handlers[h])();

	return handler.command_name();
}

static void hsv_cli_exec_help(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs)
{
	char help_prompt[512] = "List of available commands:\r\n"
							"help - print information about available commands\r\n";		
	int max_len = sizeof(help_prompt) - strlen(help_prompt) - 1;


	for (int i = 0; i < HSV_CLI_COMMAND_HELP && max_len > 0; i++)
	{
		hsv_cli_handler_i handler = (*command_handlers[i])();
		const char *command_prompt = handler.help();

		strncat(help_prompt, command_prompt, max_len);
		max_len -= strlen(command_prompt);
		strncat(help_prompt, "\r\n", max_len);
		max_len -= 2;
	}
	help_prompt[sizeof(help_prompt) - 1] = '\0';

	cdc_acm_write(cdc_acm, help_prompt, strlen(help_prompt));
}

hsv_cli_command_t hsv_cli_resolve_command(const char *command)
{
	for (int i = 0; i < HSV_CLI_COMMAND_HELP; i++)
	{
		const char *cmd_str = hsv_cli_get_cmd_str(i);
		if (strcmp(command, cmd_str) == 0)
		{
			return i;
		}
	}

	if (strcmp(command, "help") == 0)
	{
		return HSV_CLI_COMMAND_HELP;
	}

	return HSV_CLI_COMMAND_UNKNOWN;
}

estc_cli_error_t hsv_cli_exec_handler(hsv_cli_command_t h,
						              app_usbd_cdc_acm_t const *cdc_acm,
						              char args[][HSV_CLI_MAX_WORD_SIZE],
						              uint8_t nargs)
{
	if (h == HSV_CLI_COMMAND_HELP)
	{
		hsv_cli_exec_help(cdc_acm, args, nargs);
		return ESTC_CLI_SUCCESS;
	}

	if (h == HSV_CLI_COMMAND_UNKNOWN)
	{
		return ESTC_CLI_ERROR_UNKNOWN_COMMAND;
	}

	hsv_cli_handler_i handler = (*command_handlers[h])();

	return handler.command_handler(cdc_acm, args, nargs);
}
