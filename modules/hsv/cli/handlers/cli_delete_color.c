#include "include/cli_delete_color.h"
#include "utils/cli_colors_utils.h"
#include "modules/hsv/cli/handlers/utils/cli_utils.h"
#include "utils/hash/hash.h"
#include "storage/color/hsv_saved_colors.h"

static const char *HSV_CLI_CANNOT_FIND_COLOR_PROMPT = "Cannot find specified saved color.\r\n";

hsv_cli_handler_i hsv_cli_delete_color_command(void)
{
	return (hsv_cli_handler_i) {
		.command_handler = hsv_cli_handler_delete_color,
		.command_name = hsv_cli_handler_delete_color_name,
		.help = hsv_cli_handler_delete_color_help,
	};
}

void hsv_cli_handler_delete_color(app_usbd_cdc_acm_t const *cdc_acm, 
                                  char args[][HSV_CLI_MAX_WORD_SIZE], 
							      uint8_t nargs)
{
	if (nargs != 1)
	{
		hsv_cli_utils_invalid_number_of_args_prompt(cdc_acm, __func__, 1, nargs);
		return;
	}
	
	hsv_cli_load_colors();

	uint32_t color_name_hash = utils_hash_str_jenkins(args[0]);
	hsv_saved_colors_err_t err = hsv_saved_colors_delete(color_name_hash);
	if (err == HSV_SAVED_COLORS_SUCCESS)
	{
		return;
	}

	cdc_acm_write(cdc_acm, HSV_CLI_CANNOT_FIND_COLOR_PROMPT, strlen(HSV_CLI_CANNOT_FIND_COLOR_PROMPT));
}

const char *hsv_cli_handler_delete_color_help(void)
{
	return "del_color <color_name> - delete already saved color";
}

const char *hsv_cli_handler_delete_color_name(void)
{
	return "del_color";
}
