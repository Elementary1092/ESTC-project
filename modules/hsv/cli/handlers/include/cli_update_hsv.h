#ifndef HSV_CLI_UPDATE_HSV_HANDLER_H
#define HSV_CLI_UPDATE_HSV_HANDLER_H

#include "cli_handler_interface.h"

/**
 * @brief Getter of update_hsv command.
*/
hsv_cli_handler_i hsv_cli_update_hsv(void);

/**
 * @brief Handler of update_hsv command.
 * 
 * @param[in] cdc_acm Registered cdc_acm instance where output will be written.
 * 
 * @param[in] args Command arguments.
 * 
 * @param[in] nargs Number of arguments.
 * 
 * @note Immediatelly returns if number of arguments is incorrect.
 * 
 * @note Expects 3 arguments:
 *       - Hue;
 *       - Saturation;
 *       - Brightness.
*/
estc_cli_error_t hsv_cli_handler_update_hsv(app_usbd_cdc_acm_t const *cdc_acm, 
                                            char args[][HSV_CLI_MAX_WORD_SIZE], 
							                uint8_t nargs);

/**
 * @brief Returns update_hsv command help prompt.
*/
const char *hsv_cli_handler_update_hsv_help(void);

/**
 * @brief Returns update_hsv command actual name.
*/
const char *hsv_cli_handler_update_hsv_name(void);

#endif
