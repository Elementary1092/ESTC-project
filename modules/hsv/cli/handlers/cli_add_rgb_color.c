#include "include/cli_add_rgb_color.h"
#include "modules/hsv/cli/handlers/utils/cli_utils.h"
#include "modules/hsv/cli/handlers/utils/cli_colors_utils.h"
#include "utils/strings/strings.h"

hsv_cli_handler_i hsv_cli_add_rgb_color(void)
{
	return (hsv_cli_handler_i) {
		.command_handler = hsv_cli_handler_add_rgb_color,
		.help = hsv_cli_handler_add_rgb_color_help,
		.command_name = hsv_cli_handler_add_rgb_color_name,
	};
}

estc_cli_error_t hsv_cli_handler_add_rgb_color(char args[][HSV_CLI_MAX_WORD_SIZE], 
    						                   uint8_t nargs)
{
	if (nargs != 4)
	{
		return ESTC_CLI_ERROR_INVALID_NUMBER_OF_ARGS;
	}
	
	hsv_cli_load_colors();

	uint32_t red = utils_strings_atou(args[0]);
	uint32_t green = utils_strings_atou(args[1]);
	uint32_t blue = utils_strings_atou(args[2]);

	return hsv_cli_save_color(red, green, blue, args[3]);
}

const char *hsv_cli_handler_add_rgb_color_help(void)
{
	return "add_rgb_color <R> <G> <B> <name> - "
	       "save red, green, blue combination";
}

const char *hsv_cli_handler_add_rgb_color_name(void)
{
	return "add_rgb_color";
}
