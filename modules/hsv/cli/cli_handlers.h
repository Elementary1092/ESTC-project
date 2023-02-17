#ifndef HSV_CLI_HANDLERS_H
#define HSV_CLI_HANDLERS_H

#include <app_usbd_cdc_acm.h>
#include <stdint.h>
#include "app_config.h"

#include "modules/cli_errors/cli_errors.h"
#include "cli/handlers/include/cli_add_current_color.h"
#include "cli/handlers/include/cli_add_rgb_color.h"
#include "cli/handlers/include/cli_apply_color.h"
#include "cli/handlers/include/cli_delete_color.h"
#include "cli/handlers/include/cli_handler_interface.h"
#include "cli/handlers/include/cli_update_hsv.h"
#include "cli/handlers/include/cli_update_rgb.h"

#ifndef HSV_CLI_MAX_WORD_SIZE
#define HSV_CLI_MAX_WORD_SIZE ESTC_MAX_LINE_SIZE
#endif

#ifndef HSV_CLI_SAVED_COLORS_MAX_NUM
#define HSV_CLI_SAVED_COLORS_MAX_NUM 10
#endif

#define HSV_CLI_EXECUTORS_EXPAND_AS_ENUM(a, b) a,
#define HSV_CLI_EXECUTORS_EXPAND_AS_INTERFACE(a, b) b,

#define HSV_CLI_HANDLERS(EXECUTOR)                                         \
	EXECUTOR(HSV_CLI_COMMAND_UPDATE_HSV, hsv_cli_update_hsv)               \
	EXECUTOR(HSV_CLI_COMMAND_UPDATE_RGB, hsv_cli_update_rgb)               \
	EXECUTOR(HSV_CLI_COMMAND_ADD_RGB_COLOR, hsv_cli_add_rgb_color)         \
	EXECUTOR(HSV_CLI_COMMAND_APPLY_COLOR, hsv_cli_apply_color)             \
	EXECUTOR(HSV_CLI_COMMAND_DELETE_COLOR, hsv_cli_delete_color_command)   \
	EXECUTOR(HSV_CLI_COMMAND_ADD_CURRENT_COLOR, hsv_cli_add_current_color) \

/**
 * @brief Commands that may be executed this CLI.
 * 
 * @warning Do not change a position of HSV_CLI_COMMAND_HELP nor move it to X-macros defining available commands.
*/
typedef enum
{
	HSV_CLI_HANDLERS(HSV_CLI_EXECUTORS_EXPAND_AS_ENUM)
	HSV_CLI_COMMAND_HELP,
	HSV_CLI_COMMAND_UNKNOWN,
} hsv_cli_command_t;

/**
 * @brief Checks whether a handler of given command exists or not.
 * 
 * @param[in] command Command name
 * 
 * @retval @ref HSV_CLI_COMMAND_UNKNOWN such command handler does not exists.
 * 
 * @retval default enumerator describing a handler of the command.
*/
hsv_cli_command_t hsv_cli_resolve_command(const char *command);

/**
 * @brief Executes handler of a command if it exists with passed arguments.
 * 
 * @param[in] h enumerator decribing command handler.
 * 
 * @param[in] cdc_acm Registered cdc acm instance where the output will be written.
 * 
 * @param[in] args Arguments of the command (without command name).
 * 
 * @param[in] nargs Number of arguments.
*/
estc_cli_error_t hsv_cli_exec_handler(hsv_cli_command_t h,
						             app_usbd_cdc_acm_t const *cdc_acm,
						             char args[][HSV_CLI_MAX_WORD_SIZE],
						             uint8_t nargs);

#endif