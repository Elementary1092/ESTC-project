#include <nrfx_log.h>
#include <string.h>

#include "utils/strings/strings.h"
#include "utils/hash/hash.h"
#include "modules/cdc_acm/cdc_acm.h"
#include "cli_handlers.h"
#include "hsv_picker.h"
#include "hsv_saved_colors.h"

#ifndef ESTC_CLI_HELP_HEADER
#define ESTC_CLI_HELP_HEADER "List of available commands:\r\n" \
                             "help - print information about available commands\r\n"
#endif

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

static void hsv_cli_exec_help(estc_cli_error_t error, char help_prompt[512])
{
	memset(help_prompt, 0, 512);
	strncat(help_prompt, ESTC_CLI_HELP_HEADER, 511);
	size_t max_len = 511UL - strlen(help_prompt);


	for (int i = 0; i < HSV_CLI_COMMAND_HELP; i++)
	{
		hsv_cli_handler_i handler = (*command_handlers[i])();
		const char *command_prompt = handler.help();

		if (max_len < strlen(command_prompt))
		{
			break;
		}

		strncat(help_prompt, command_prompt, max_len);
		max_len -= strlen(command_prompt);

		if (max_len < 3UL)
		{
			break;
		}

		strncat(help_prompt, "\r\n", max_len);
		max_len -= 2;
	}
	help_prompt[511] = '\0';
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

estc_cli_described_result_t hsv_cli_exec_handler(hsv_cli_command_t h,
						                         char args[][HSV_CLI_MAX_WORD_SIZE],
						                         uint8_t nargs)
{
	if (h == HSV_CLI_COMMAND_HELP)
	{
		return (estc_cli_described_result_t) {
			.error = ESTC_CLI_SUCCESS,
			.result_describer = hsv_cli_exec_help,
		};
	}

	if (h == HSV_CLI_COMMAND_UNKNOWN)
	{
		return (estc_cli_described_result_t){
			.error = ESTC_CLI_ERROR_UNKNOWN_COMMAND,
			.result_describer = estc_cli_describe_error,
		};
	}

	hsv_cli_handler_i handler = (*command_handlers[h])();

	estc_cli_error_t error = handler.command_handler(args, nargs);

	return (estc_cli_described_result_t) {
		.error = error,
		.result_describer = estc_cli_describe_error,
	};
}
