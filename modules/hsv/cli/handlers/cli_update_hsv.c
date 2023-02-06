#include <nrf_log.h>

#include "include/cli_update_hsv.h"
#include "modules/hsv/cli/handlers/utils/cli_utils.h"
#include "modules/hsv/hsv_picker.h"

hsv_cli_handler_i hsv_cli_update_hsv(void)
{
	return (hsv_cli_handler_i) {
		.command_handler = hsv_cli_handler_update_hsv,
		.command_name = hsv_cli_handler_update_hsv_name,
		.help = hsv_cli_handler_update_hsv_help,
	};
}

void hsv_cli_handler_update_hsv(app_usbd_cdc_acm_t const *cdc_acm,
							 char args[][HSV_CLI_MAX_WORD_SIZE],
							 uint8_t nargs)
{
	if (nargs != 3)
	{
		hsv_cli_utils_invalid_number_of_args_prompt(cdc_acm, __func__, 3U, nargs);
		return;
	}

	uint32_t hsv_args[3];
	hsv_cli_utils_convert_strs_to_uints(hsv_args, args, nargs);

	hsv_picker_set_hsv((float)hsv_args[0], (float)hsv_args[1], (float)hsv_args[2]);
	NRF_LOG_INFO("hsv_cli_exec_update_hsv: Updated hsv");
}

const char *hsv_cli_handler_update_hsv_help(void)
{
	return "hsv <hue> <saturation> <brightness> - set hue, saturation, and brightness of RGB LEDs";
}

const char *hsv_cli_handler_update_hsv_name(void)
{
	return "hsv";
}
