#ifndef HSV_CLI_DELETE_COLOR_COMMAND_HANDLER_H
#define HSV_CLI_DELETE_COLOR_COMMAND_HANDLER_H

#include "cli_handler_interface.h"

/**
 * @brief Getter of delete_color command.
*/
hsv_cli_handler_i hsv_cli_delete_color_command(void);

/**
 * @brief Handler of delete_color command.
 * 
 * @param[in] args Command arguments.
 * 
 * @param[in] nargs Number of arguments.
 * 
 * @note Immediatelly returns if the number of arguments is incorrect.
 * 
 * @note Expects 1 argument: the name of the color to be deleted.
*/
estc_cli_error_t hsv_cli_handler_delete_color(char args[][HSV_CLI_MAX_WORD_SIZE], 
							                  uint8_t nargs);

/**
 * @brief Returns delete_color help prompt.
*/
const char *hsv_cli_handler_delete_color_help(void);

/**
 * @brief Returns actual name of delete color command.
*/
const char *hsv_cli_handler_delete_color_name(void);

#endif
