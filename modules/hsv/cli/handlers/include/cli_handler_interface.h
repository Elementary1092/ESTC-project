#ifndef HSV_CLI_HANDLER_INTERFACE_H
#define HSV_CLI_HANDLER_INTERFACE_H

#include <app_usbd_cdc_acm.h>
#include <stdint.h>
#include "app_config.h"

#ifndef HSV_CLI_MAX_WORD_SIZE
#define HSV_CLI_MAX_WORD_SIZE ESTC_MAX_LINE_SIZE
#endif

/**
 * @brief Handler of a cli command.
 * 
 * @param[in] cdc_acm Registered cdc_acm instance where output will be written.
 * 
 * @param[in] args Command arguments.
 * 
 * @param[in] nargs Number of arguments.
*/
typedef void (* hsv_cli_command_handler_t)(app_usbd_cdc_acm_t const *cdc_acm, 
                                           char args[][HSV_CLI_MAX_WORD_SIZE],
										   uint8_t nargs);

/**
 * @brief Get help prompt of a command.
*/
typedef const char *(* hsv_cli_command_help_handler_t)(void);

/**
 * @brief Get the name of a command.
*/
typedef const char *(* hsv_cli_command_name_t)(void);

/**
 * @brief Interface defining a cli command.
*/
typedef struct
{
	hsv_cli_command_name_t         command_name;
	hsv_cli_command_handler_t      command_handler;
	hsv_cli_command_help_handler_t help;
} hsv_cli_handler_i;

/**
 * @brief Getter of a cli command handler.
*/
typedef hsv_cli_handler_i (* hsv_cli_command_handler_get) (void);

#endif