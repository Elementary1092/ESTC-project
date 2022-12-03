#include <nrfx_log.h>

#include "utils/strings/strings.h"
#include "utils/hash/hash.h"
#include "modules/cdc_acm/cdc_acm.h"
#include "cli_handlers.h"
#include "hsv_picker.h"

#define HSV_CLI_UNKNOWN_COMMAND_PROMPT "Unknown command. Try again.\r\n"
#define HSV_CLI_SAVED_COLORS_BUF_FULL_PROMPT "Cannot save color. Already saved maximum number of colors.\r\n"
#define HSV_CLI_CANNOT_FIND_COLOR_PROMPT "Cannot find specified saved color.\r\n"

static hsv_cli_command_handler_t handlers[] = 
{
	HSV_CLI_HANDLERS(HSV_CLI_HANDLERS_EXPAND_AS_FNS)
};

static char cmd_strs[][HSV_CLI_MAX_WORD_SIZE] =
{
	HSV_CLI_HANDLERS(HSV_CLI_HANDLERS_EXPAND_AS_CMD_STR)
};

static uint32_t saved_colors[HSV_CLI_SAVED_COLORS_MAX_NUM][3] = {0U};
static uint32_t saved_color_name_hashes[HSV_CLI_SAVED_COLORS_MAX_NUM] = {0U};
static uint32_t saved_colors_idx = 0U;

static void hsv_cli_convert_nstrs_to_nuints(uint32_t *converted_args, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs)
{
	for (uint8_t i = 0; i < nargs; i++)
	{
		converted_args[i] = utils_strings_atou(args[i]);
	}
}

static void hsv_cli_delete_saved_color(uint32_t color_idx)
{
	if (saved_colors_idx == 0)
	{
		return;
	}

	saved_colors_idx--;
	for (uint32_t i = color_idx; i < saved_colors_idx; i++)
	{
		for (uint32_t j = 0UL; j < 3UL; j++)
		{
			saved_colors[i][j] = saved_colors[i+1][j];
		}
		saved_color_name_hashes[i] = saved_color_name_hashes[i+1];
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
						"\t3. rgb <red> <green> <blue> - set values of RGB LEDs.\r\n"
						"\t4. add_rgb_color <red> <green> <blue> <color_name> - save red, green, blue combination under color_name\r\n"
						"\t5. apply_color <color_name> - try applying color saved as <color_name>\r\n";

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
		NRFX_LOG_ERROR("hsv_cli_exec_update_rgb: Invalid number of args: %u", nargs);
		return;
	}

	uint32_t rgb_args[3];
	hsv_cli_convert_nstrs_to_nuints(rgb_args, args, nargs);

	hsv_picker_set_rgb(rgb_args[0], rgb_args[1], rgb_args[2]);
	NRFX_LOG_INFO("hsv_cli_exec_update_rgb: Updated rgb");
}

void hsv_cli_exec_add_rgb_color(app_usbd_cdc_acm_t const *cdc_acm, 
								char args[][HSV_CLI_MAX_WORD_SIZE], 
								uint8_t nargs)
{
	if (nargs != 4)
	{
		NRFX_LOG_ERROR("hsv_cli_exec_add_rgb_color: Invalid number of args: %u", nargs);
		return;
	}

	if (saved_colors_idx >= HSV_CLI_SAVED_COLORS_MAX_NUM)
	{
		cdc_acm_write(cdc_acm, HSV_CLI_SAVED_COLORS_BUF_FULL_PROMPT, strlen(HSV_CLI_SAVED_COLORS_BUF_FULL_PROMPT));
		return;
	}

	saved_color_name_hashes[saved_colors_idx] = utils_hash_str_jenkins(args[3]);
	for (size_t i = 0UL; i < 3UL; i++)
	{
		saved_colors[saved_colors_idx][i] = utils_strings_atou(args[i]);
	}
	saved_colors_idx++;
}

void hsv_cli_exec_apply_color(app_usbd_cdc_acm_t const *cdc_acm, 
							  char args[][HSV_CLI_MAX_WORD_SIZE], 
							  uint8_t nargs)
{
	if (nargs != 1)
	{
		NRFX_LOG_ERROR("hsv_cli_exec_apply_color: Invalid number of args: %u", nargs);
		return;
	}

	uint32_t color_name_hash = utils_hash_str_jenkins(args[0]);
	for (uint32_t i = 0; i < saved_colors_idx; i++)
	{
		if (saved_color_name_hashes[i] == color_name_hash)
		{
			hsv_picker_set_rgb(saved_colors[i][0], saved_colors[i][1], saved_colors[i][2]);
			return;
		}
	}

	cdc_acm_write(cdc_acm, HSV_CLI_CANNOT_FIND_COLOR_PROMPT, strlen(HSV_CLI_CANNOT_FIND_COLOR_PROMPT));
}

void hsv_cli_exec_del_color(app_usbd_cdc_acm_t const *cdc_acm, 
							char args[][HSV_CLI_MAX_WORD_SIZE], 
							uint8_t nargs)
{
	if (nargs != 1)
	{
		NRFX_LOG_ERROR("hsv_cli_exec_del: Invalid number of args: %u", nargs);
		return;
	}

	uint32_t color_name_hash = utils_hash_str_jenkins(args[0]);
	for (uint32_t i = 0; i < saved_colors_idx; i++)
	{
		if (saved_color_name_hashes[i] == color_name_hash)
		{
			hsv_cli_delete_saved_color(i);
			return;
		}
	}

	cdc_acm_write(cdc_acm, HSV_CLI_CANNOT_FIND_COLOR_PROMPT, strlen(HSV_CLI_CANNOT_FIND_COLOR_PROMPT));
}
