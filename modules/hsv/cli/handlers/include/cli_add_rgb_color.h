#ifndef HSV_CLI_ADD_RGB_COLOR_COMMAND_HANDLER_H
#define HSV_CLI_ADD_RGB_COLOR_COMMAND_HANDLER_H

#include "cli_handler_interface.h"

/**
 * @brief Getter of add_rgb_color command
*/
hsv_cli_handler_i hsv_cli_add_rgb_color(void);

/**
 * @brief Handler of add_rgb_color command.
 * 
 * @param[in] cdc_acm Registered cdc_acm instance where output will be written.
 * 
 * @param[in] args Command arguments.
 * 
 * @param[in] nargs Number of arguments.
 * 
 * @note Immediatelly returns if the number of arguments is incorrect.
 * 
 * @note Expectes 4 arguments:
 *       - RED;
 *       - GREEN;
 *       - BLUE;
 *       - COLOR_NAME.
*/
void hsv_cli_handler_add_rgb_color(app_usbd_cdc_acm_t const *cdc_acm, 
                                   char args[][HSV_CLI_MAX_WORD_SIZE], 
    						       uint8_t nargs);

/**
 * @brief Returns add_rgb_color command help prompt.
*/
const char *hsv_cli_handler_add_rgb_color_help(void);

/**
 * @brief Returns actual name of add_rgb_color command.
*/
const char *hsv_cli_handler_add_rgb_color_name(void);

#endif
