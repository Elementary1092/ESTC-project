#ifndef HSV_CLI_ADD_CURRENT_COLOR_COMMAND_HANDLER_H
#define HSV_CLI_ADD_CURRENT_COLOR_COMMAND_HANDLER_H

#include "cli_handler_interface.h"

/**
 * @brief Getter of add_current_color command.
*/
hsv_cli_handler_i hsv_cli_add_current_color(void);

/**
 * @brief Handler of add_current_color command.
 * 
 * @param[in] args Command arguments.
 * 
 * @param[in] nargs Number of arguments.
 * 
 * @note Immediatelly returns if the number of arguments is incorrect. Expects 1 argument (color name).
*/
estc_cli_error_t hsv_cli_handler_add_current_color(char args[][HSV_CLI_MAX_WORD_SIZE], 
							                       uint8_t nargs);

/**
 * @brief Returns help prompt of add_current_color command.
*/
const char *hsv_cli_handler_add_current_color_help(void);

/**
 * @brief Returns the actual name of add_current_color command.
*/
const char *hsv_cli_handler_add_current_color_name(void);

#endif
