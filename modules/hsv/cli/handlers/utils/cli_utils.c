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
