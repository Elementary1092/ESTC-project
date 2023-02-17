#include <stdbool.h>

#include "cli_colors_utils.h"
#include "storage/color/hsv_saved_colors.h"
#include "utils/hash/hash.h"

#define HSV_CLI_SAVED_COLOR_PROMPT "Saved color successfully.\r\n"
#define HSV_CLI_SAVED_COLORS_BUF_FULL_PROMPT "Cannot save color. Already saved maximum number of colors.\r\n"
#define HSV_CLI_FAILED_TO_SAVE_COLOR_PROMPT "Failed to save color.\r\n"

static bool loaded_saved_colors = false;

estc_cli_error_t hsv_cli_save_color(uint32_t red, 
						            uint32_t green, 
						            uint32_t blue, 
						            const char *color_name)
{
	uint32_t color_name_hash = utils_hash_str_jenkins(color_name);
	hsv_saved_color_rgb_t rgb = {
		.hash = color_name_hash,
		.red = red,
		.green = green,
		.blue = blue,
	};
	hsv_saved_colors_err_t err = hsv_saved_colors_add_or_mod(&rgb);
	
	if (err == HSV_SAVED_COLORS_SUCCESS)
	{
		return ESTC_CLI_SUCCESS;
	}
	else if (err == HSV_SAVED_COLORS_NO_COLOR_SLOT)
	{
		return ESTC_CLI_ERROR_BUFFER_OVERFLOW;
	}
	else
	{
		return ESTC_CLI_ERROR_FAILED_TO_SAVE;
	}
}

void hsv_cli_load_colors(void)
{
	if (loaded_saved_colors)
	{
		return;
	}

	hsv_saved_colors_load();
	loaded_saved_colors = true;
}