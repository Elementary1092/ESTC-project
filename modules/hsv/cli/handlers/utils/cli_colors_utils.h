#ifndef HSV_CLI_COLORS_UTILS_H
#define HSV_CLI_COLORS_UTILS_H

#include <stdint.h>
#include "modules/cli_errors/cli_errors.h"

/**
 * @brief Triggers loading of saved colors if it was not triggered yet.
 * 
 * @note Recommended to call it every time before working with saved colors.
*/
void hsv_cli_load_colors(void);

/**
 * @brief Hashes color name and saves rgb color with its hash.
 * 
 * @param[in] red Red value of rgb (expected to be in a range 0 - 255)
 * 
 * @param[in] green Green value of rgb (expected to be in a range 0 - 255)
 *
 * @param[in] blue Blue value of rgb (expected to be in a range 0 - 255)
 * 
 * @param[in] color_name A name of a new color
 * 
 * @retval ESTC_CLI_SUCCESS when color is saved successfully
 * 
 * @retval ESTC_CLI_ERROR_BUFFER_OVERFLOW when maximum number of color is already saved
 * 
 * @retval ESTC_CLI_ERROR_FAILED_TO_SAVE on other error on saving new color
*/
estc_cli_error_t hsv_cli_save_color(uint32_t red, 
						            uint32_t green, 
						            uint32_t blue, 
						            const char *color_name);

#endif