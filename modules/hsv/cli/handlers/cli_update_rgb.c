#include "include/cli_update_rgb.h"
#include "modules/hsv/cli/handlers/utils/cli_utils.h"
#include "modules/hsv/hsv_picker.h"

hsv_cli_handler_i hsv_cli_update_rgb(void)
{
	return (hsv_cli_handler_i) {
		.command_handler = hsv_cli_handler_update_rgb,
		.command_name = hsv_cli_handler_update_rgb_name,
		.help = hsv_cli_handler_update_rgb_help,
	};
}

estc_cli_error_t hsv_cli_handler_update_rgb(char args[][HSV_CLI_MAX_WORD_SIZE], 
							                uint8_t nargs)
{
	if (nargs != 3)
	{
		return ESTC_CLI_ERROR_INVALID_NUMBER_OF_ARGS;
	}

	uint32_t rgb_args[3];
	hsv_cli_utils_convert_strs_to_uints(rgb_args, args, nargs);

	hsv_picker_set_rgb(rgb_args[0], rgb_args[1], rgb_args[2]);

	return ESTC_CLI_SUCCESS;
}

const char *hsv_cli_handler_update_rgb_help(void)
{
	return "rgb <R> <G> <B> - set red, green, blue, values";
}

const char *hsv_cli_handler_update_rgb_name(void)
{
	return "rgb";
}
