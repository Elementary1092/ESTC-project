#ifndef HSV_CLI_UPDATE_RGB_HANDLER_H
#define HSV_CLI_UPDATE_RGB_HANDLER_H

#include "cli_handler_interface.h"

/**
 * @brief Getter of update_rgb handler.
*/
hsv_cli_handler_i hsv_cli_update_rgb(void);

/**
 * @brief Handler of update_rgb command.
 * 
 * @param[in] args Command arguments.
 * 
 * @param[in] nargs Number of arguments.
 * 
 * @note Immediatelly returns if number of arguments is incorrect.
 * 
 * @note Expects 3 arguments:
 *       - Red;
 *       - Green;
 *       - Blue;
*/
estc_cli_error_t hsv_cli_handler_update_rgb(char args[][HSV_CLI_MAX_WORD_SIZE], 
							                uint8_t nargs);

/**
 * @brief Returns help prompt of update_rgb command.
*/
const char *hsv_cli_handler_update_rgb_help(void);

/**
 * @brief Returns actual name of update_rgb command.
*/
const char *hsv_cli_handler_update_rgb_name(void);

#endif
