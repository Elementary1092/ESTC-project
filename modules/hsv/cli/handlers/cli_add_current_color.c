#include "include/cli_add_current_color.h"
#include "modules/hsv/cli/handlers/utils/cli_utils.h"
#include "utils/cli_colors_utils.h"
#include "storage/color/hsv_saved_colors.h"
#include "modules/hsv/hsv_picker.h"

hsv_cli_handler_i hsv_cli_add_current_color(void)
{
	return (hsv_cli_handler_i) {
		.command_handler = hsv_cli_handler_add_current_color,
		.help = hsv_cli_handler_add_current_color_help,
		.command_name = hsv_cli_handler_add_current_color_name,
	};
}

estc_cli_error_t hsv_cli_handler_add_current_color(char args[][HSV_CLI_MAX_WORD_SIZE], 
							                       uint8_t nargs)
{
	if (nargs != 1)
	{
		return ESTC_CLI_ERROR_INVALID_NUMBER_OF_ARGS;
	}

	hsv_cli_load_colors();

	rgb_value_t curr_rgb;
	hsv_picker_get_current_rgb(&curr_rgb);

	uint32_t red = (uint32_t)(curr_rgb.red);
	uint32_t green = (uint32_t)(curr_rgb.green);
	uint32_t blue = (uint32_t)(curr_rgb.blue);

	return hsv_cli_save_color(red, green, blue, args[0]);
}

const char *hsv_cli_handler_add_current_color_help(void)
{
	return "add_current_color <color_name> - save currently displayed color";
}

const char *hsv_cli_handler_add_current_color_name(void)
{
	return "add_current_color";
}
