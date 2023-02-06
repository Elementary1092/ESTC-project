#ifndef HSV_CLI_UNKNOWN_COMMAND_HANDLER_H
#define HSV_CLI_UNKNOWN_COMMAND_HANDLER_H

#include "cli_handler_interface.h"

/**
 * @brief Getter of unknown command handler.
*/
hsv_cli_handler_i hsv_cli_unknown_command(void);

/**
 * @brief Handler of unknown command.
 * 
 * @param[in] cdc_acm Registered cdc_acm instance where output will be written.
 * 
 * @param[in] args Command arguments.
 * 
 * @param[in] nargs Number of arguments.
 * 
 * @note Accepts any number of arguments but ignores them.
*/
void hsv_cli_handler_unknown(app_usbd_cdc_acm_t const *cdc_acm, 
                             char args[][HSV_CLI_MAX_WORD_SIZE], 
							 uint8_t nargs);

/**
 * @brief Returns an empty string.
*/
const char *hsv_cli_handler_unknown_command_help(void);

/**
 * @brief Returns an empty string.
*/
const char *hsv_cli_handler_unknown_command_name(void);

#endif