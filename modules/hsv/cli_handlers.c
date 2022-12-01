#include <nrfx_log.h>

#include "../../utils/strings/strings.h"
#include "../cdc_acm/cdc_acm.h"
#include "cli_handlers.h"
#include "hsv_picker.h"

#define HSV_CLI_UNKNOWN_COMMAND_PROMPT "Unknown command. Try again.\r\n"

static hsv_cli_command_handler_t handlers[] = 
{
	HSV_CLI_HANDLERS(HSV_CLI_HANDLERS_EXPAND_AS_FNS)
};

static char cmd_strs[][HSV_CLI_MAX_WORD_SIZE] =
{
	HSV_CLI_HANDLERS(HSV_CLI_HANDLERS_EXPAND_AS_CMD_STR)
};

static void hsv_cli_convert_nstrs_to_nuints(uint32_t *converted_args, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs)
{
	for (uint8_t i = 0; i < nargs; i++)
	{
		converted_args[i] = utils_strings_atou(args[i]);
	}
}

char *hsv_cli_get_cmd_str(hsv_cli_command_t h)
{
	return cmd_strs[h];
}

void hsv_cli_exec_handler(hsv_cli_command_t h, app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs)
{
	(*handlers[h])(cdc_acm, args, nargs);
}

void hsv_cli_exec_unknown(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs)
{
	cdc_acm_write(cdc_acm, HSV_CLI_UNKNOWN_COMMAND_PROMPT, strlen(HSV_CLI_UNKNOWN_COMMAND_PROMPT));
}

void hsv_cli_exec_help(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs)
{
	const char *help_prompt = 
						"Available commands:\r\n"
						"\t1. help - get list of available commands;\r\n"
						"\t2. hsv <hue> <saturation> <brightness> - set hue, saturation, and brightness of RGB LEDs;\r\n"
						"\t3. rgb <red> <green> <blue> - set values of RGB LEDs.\r\n";

	cdc_acm_write(cdc_acm, help_prompt, strlen(help_prompt));
}

void hsv_cli_exec_update_hsv(app_usbd_cdc_acm_t const *cdc_acm, 
							char args[][HSV_CLI_MAX_WORD_SIZE], 
							uint8_t nargs)
{
	if (nargs != 3)
	{
		NRFX_LOG_ERROR("hsv_cli_exec_update_hsv: Invalid number of args: %u", nargs);
		return;
	}

	uint32_t hsv_args[3];
	hsv_cli_convert_nstrs_to_nuints(hsv_args, args, nargs);

	hsv_picker_set_hsv((float)hsv_args[0], (float)hsv_args[1], (float)hsv_args[2]);
	NRFX_LOG_INFO("hsv_cli_exec_update_hsv: Updated hsv");
}


void hsv_cli_exec_update_rgb(app_usbd_cdc_acm_t const *cdc_acm, 
							char args[][HSV_CLI_MAX_WORD_SIZE], 
							uint8_t nargs)
{
	if (nargs != 3)
	{
		NRFX_LOG_ERROR("hsv_cli_exec_update_hsv: Invalid number of args: %u", nargs);
		return;
	}

	uint32_t rgb_args[3];
	hsv_cli_convert_nstrs_to_nuints(rgb_args, args, nargs);

	hsv_picker_set_rgb(rgb_args[0], rgb_args[1], rgb_args[2]);
	NRFX_LOG_INFO("hsv_cli_exec_update_rgb: Updated rgb");
}
