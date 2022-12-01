#include <nrf_log.h>
#include <app_usbd_cdc_acm.h>
#include <string.h>
#include <stdlib.h>

#include "cli.h"
#include "hsv_picker.h"
#include "../../utils/strings/strings.h"
#include "../cdc_acm/cdc_acm.h"
#include "app_config.h"

#define HSV_CLI_MAX_ARGS      5
#define HSV_CLI_MAX_WORD_SIZE ESTC_MAX_LINE_SIZE

#define HSV_CLI_UNKNOWN_COMMAND_PROMPT "Unknown command. Try again.\r\n"

static hsv_cli_command_t hsv_cli_resolve_command(char const *str);
static void hsv_cli_exec_help(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);
static void hsv_cli_exec_update_hsv(char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);
static void hsv_cli_convert_nstrs_to_nuints(uint32_t *converted_args, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);
static void hsv_cli_exec_update_rgb(char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

static hsv_cli_command_t hsv_cli_resolve_command(char const *str)
{
	if (strcmp(str, HSV_CLI_COMMAND_HELP_STR) == 0)
	{
		return HSV_CLI_COMMAND_HELP;
	}
	else if (strcmp(str, HSV_CLI_COMMAND_HSV_STR) == 0)
	{
		return HSV_CLI_COMMAND_UPDATE_HSV;
	}
	else if (strcmp(str, HSV_CLI_COMMAND_RGB_STR) == 0)
	{
		return HSV_CLI_COMMAND_UPDATE_RGB;
	}

	return HSV_CLI_COMMAND_UNKNOWN;
}

static void hsv_cli_exec_help(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs)
{
	const char *help_prompt = 
						"Available commands:\r\n"
						"\t1. help - get list of available commands;\r\n"
						"\t2. hsv <hue> <saturation> <brightness> - set hue, saturation, and brightness of RGB LEDs;\r\n"
						"\t3. rgb <red> <green> <blue> - set values of RGB LEDs.\r\n";

	cdc_acm_write(cdc_acm, help_prompt, strlen(help_prompt));
}

static void hsv_cli_exec_update_hsv(char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs)
{
	if (nargs != 3)
	{
		NRF_LOG_ERROR("hsv_cli_exec_update_hsv: Invalid number of args: %u", nargs);
		return;
	}

	uint32_t hsv_args[3];
	hsv_cli_convert_nstrs_to_nuints(hsv_args, args, nargs);

	hsv_picker_set_hsv((float)hsv_args[0], (float)hsv_args[1], (float)hsv_args[2]);
	NRF_LOG_INFO("hsv_cli_exec_update_hsv: Updated hsv");
}

static void hsv_cli_exec_update_rgb(char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs)
{
	if (nargs != 3)
	{
		NRF_LOG_ERROR("hsv_cli_exec_update_hsv: Invalid number of args: %u", nargs);
		return;
	}

	uint32_t rgb_args[3];
	hsv_cli_convert_nstrs_to_nuints(rgb_args, args, nargs);

	hsv_picker_set_rgb(rgb_args[0], rgb_args[1], rgb_args[2]);
	NRF_LOG_INFO("hsv_cli_exec_update_rgb: Updated rgb");
}

static void hsv_cli_convert_nstrs_to_nuints(uint32_t *converted_args, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t args_count)
{
	for (uint8_t i = 0; i < args_count; i++)
	{
		converted_args[i] = utils_strings_atou(args[i]);
	}
}

void hsv_cli_exec_command(app_usbd_cdc_acm_t const *cdc_acm,
						  cdc_acm_read_buf_ctx_t *read_buf)
{
	char args[HSV_CLI_MAX_ARGS + 1][HSV_CLI_MAX_WORD_SIZE] = {0};
	size_t nargs = utils_strings_tokenize(args, read_buf->buf, " \r\n\t\0");

	if (nargs == 0)
	{
		return;
	}
	
	hsv_cli_command_t cmd = hsv_cli_resolve_command(args[0]);
	
	switch (cmd)
	{
		case HSV_CLI_COMMAND_UNKNOWN:
			cdc_acm_write(cdc_acm, HSV_CLI_UNKNOWN_COMMAND_PROMPT, strlen(HSV_CLI_UNKNOWN_COMMAND_PROMPT));
			break;

		case HSV_CLI_COMMAND_HELP:
			hsv_cli_exec_help(cdc_acm, args + 1, nargs - 1);
			break;

		case HSV_CLI_COMMAND_UPDATE_HSV:
			hsv_cli_exec_update_hsv(args + 1, nargs - 1);
			break;

		case HSV_CLI_COMMAND_UPDATE_RGB:
			hsv_cli_exec_update_rgb(args + 1, nargs - 1);
			break;

		default:
			NRF_LOG_ERROR("hsv_cli_exec_command: Unknown command: %d", cmd);
			break;
	}
}
