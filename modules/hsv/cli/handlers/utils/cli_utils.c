#include <string.h>
#include <nrfx_log.h>
#include "cli_utils.h"
#include "utils/strings/strings.h"

#define HSV_CLI_INVALID_NUM_OF_ARGS_FORMAT "Invalid number of arguments; expected: %u, got: %u\r\n"

void hsv_cli_utils_convert_strs_to_uints(uint32_t *converted_args_buffer, 
                                         char args[][HSV_CLI_UTILS_MAX_WORD_SIZE], 
										 uint8_t nargs)
{
	for (uint8_t i = 0; i < nargs; i++)
	{
		converted_args_buffer[i] = utils_strings_atou(args[i]);
	}
}

void hsv_cli_utils_invalid_number_of_args_prompt(app_usbd_cdc_acm_t const *cdc_acm,
												 const char *function_name,
                                                 uint8_t expected_args,
												 uint8_t got_args)
{
	char buffer[100] = {'\0'};
	int max_len = sizeof(buffer) - 1;
	int written = snprintf(buffer, max_len, HSV_CLI_INVALID_NUM_OF_ARGS_FORMAT, expected_args, got_args);
	if (written == max_len)
	{
		buffer[max_len] = '\0';
		written++;
	}

	NRFX_LOG_ERROR("%s: Invalid number of args: %u", function_name, got_args);
	cdc_acm_write(cdc_acm, buffer, written);
}
