#ifndef HSV_CLI_APPLY_COLOR_HANDLER_H
#define HSV_CLI_APPLY_COLOR_HANDLER_H

#include "cli_handler_interface.h"

/**
 * @brief Getter of apply_color command.
*/
hsv_cli_handler_i hsv_cli_apply_color(void);

/**
 * @brief Handler of apply_color command.
 * 
 * @param[in] cdc_acm Registered cdc_acm instance where output will be written.
 * 
 * @param[in] args Command arguments.
 * 
 * @param[in] nargs Number of arguments.
 * 
 * @note Immediatelly returns if the number of arguments is incorrect. Expects 1 argument (color name).
*/
estc_cli_error_t hsv_cli_handler_apply_color(app_usbd_cdc_acm_t const *cdc_acm, 
                                             char args[][HSV_CLI_MAX_WORD_SIZE], 
							                 uint8_t nargs);
 /**
  * @brief Returns help prompt of apply_color command.
 */
const char *hsv_cli_handler_apply_color_help(void);

/**
 * @brief Returns actual name of an apply_color command.
*/
const char *hsv_cli_handler_apply_color_name(void);

#endif
