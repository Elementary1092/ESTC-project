#ifndef HSV_CLI_COLORS_UTILS_H
#define HSV_CLI_COLORS_UTILS_H

#include "modules/cdc_acm/cdc_acm.h"

/**
 * @brief Triggers loading of saved colors if it was not triggered yet.
 * 
 * @note Recommended to call it every time before working with saved colors.
*/
void hsv_cli_load_colors(void);

/**
 * @brief Hashes color name and saves rgb color with its hash.
 * 
 * @param[in] cdc_acm Registered cdc_acm instance which will be used to write an output messages.
 * 
 * @param[in] red Red value of rgb (expected to be in a range 0 - 255)
 * 
 * @param[in] green Green value of rgb (expected to be in a range 0 - 255)
 *
 * @param[in] blue Blue value of rgb (expected to be in a range 0 - 255)
*/
void hsv_cli_save_color(app_usbd_cdc_acm_t const *cdc_acm, 
                        uint32_t red, 
						uint32_t green, 
						uint32_t blue, 
						const char *color_name);

#endif