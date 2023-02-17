#ifndef ESTC_HSV_CLI_UTILS_H
#define ESTC_HSV_CLI_UTILS_H

#include <stdint.h>
#include "modules/cdc_acm/cdc_acm.h"
#include "app_config.h"
#include "modules/hsv/cli/storage/color/hsv_saved_colors.h"

#ifndef HSV_CLI_UTILS_WORD_SIZE
#define HSV_CLI_UTILS_MAX_WORD_SIZE ESTC_MAX_LINE_SIZE
#endif

/**
 * @brief Converts some number of string to integers
 * 
 * @param[out] converted_args_buffer Buffer where all converted values will be stored.
 * 
 * @param[in] args Strings which should be converted to uint32_t
 * 
 * @param[in] nargs Number of string to be converted. 
*/
void hsv_cli_utils_convert_strs_to_uints(uint32_t *converted_args_buffer, 
                                         char args[][HSV_CLI_UTILS_MAX_WORD_SIZE], 
										 uint8_t nargs);

#endif