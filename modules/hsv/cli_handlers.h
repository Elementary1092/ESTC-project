#ifndef HSV_CLI_HANDLERS_H
#define HSV_CLI_HANDLERS_H

#include <app_usbd_cdc_acm.h>
#include <stdint.h>
#include "app_config.h"

#ifndef HSV_CLI_MAX_WORD_SIZE
#define HSV_CLI_MAX_WORD_SIZE ESTC_MAX_LINE_SIZE
#endif

#ifndef HSV_CLI_SAVED_COLORS_MAX_NUM
#define HSV_CLI_SAVED_COLORS_MAX_NUM 10
#endif

#define HSV_CLI_HANDLERS_EXPAND_AS_ENUM(a, b, c) a,
#define HSV_CLI_HANDLERS_EXPAND_AS_CMD_STR(a, b, c) b,
#define HSV_CLI_HANDLERS_EXPAND_AS_FNS(a, b, c) c,

// Using X macros to ensure that every added handler will have string to compare and enum value
#define HSV_CLI_HANDLERS(HANDLER)                                                             \
		HANDLER(HSV_CLI_CMD_UNKNOWN, "", hsv_cli_exec_unknown)                                \
		HANDLER(HSV_CLI_CMD_HELP, "help", hsv_cli_exec_help)                                  \
		HANDLER(HSV_CLI_CDM_UPDATE_HSV, "hsv", hsv_cli_exec_update_hsv)                       \
		HANDLER(HSV_CLI_CDM_UPDATE_RGB, "rgb", hsv_cli_exec_update_rgb)                       \
		HANDLER(HSV_CLI_CMD_ADD_RGB_COLOR, "add_rgb_color", hsv_cli_exec_add_rgb_color)       \
		HANDLER(HSV_CLI_CMD_APPLY_COLOR, "apply_color", hsv_cli_exec_apply_color)             \
		HANDLER(HSV_CLI_CMD_DEL_COLOR, "del_color", hsv_cli_exec_del_color)                   \
		HANDLER(HSV_CLI_CMD_ADD_CURR_COLOR, "add_current_color", hsv_cli_exec_add_curr_color)

typedef enum
{
	HSV_CLI_HANDLERS(HSV_CLI_HANDLERS_EXPAND_AS_ENUM)
	HSV_CLI_COMMAND_END,
} hsv_cli_command_t;

typedef void (* hsv_cli_command_handler_t)(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

/*
	hsv_cli_get_cmd_str returns string expected to execute { h } hsv_cli_command_t.
*/
char *hsv_cli_get_cmd_str(hsv_cli_command_t h);

/*
	hsv_cli_exec_handler executes handler of { h } command with { args } arguments.
	Output will be written to { cdc_acm }.
	{ nargs } - specifies number of arguments of the command.
*/
void hsv_cli_exec_handler(hsv_cli_command_t h, 
						  app_usbd_cdc_acm_t const *cdc_acm,
						  char args[][HSV_CLI_MAX_WORD_SIZE], 
						  uint8_t nargs);

/*
	hsv_cli_exec_unknown is triggered when command to be executed is not in the list of available commands.
*/
void hsv_cli_exec_unknown(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

/*
	hsv_cli_exec_help is triggered when command to be executed is 'help'.
	All arguments are ignored.
*/
void hsv_cli_exec_help(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

/*
	hsv_cli_exec_update_hsv is triggered when command to be executed is 'hsv'.
	Requires 3 arguments:
		1. Hue - range from 0 to 360.
		2. Saturation - range from 0 to 100.
		3. Brightness or value - range from 0 to 100.
*/
void hsv_cli_exec_update_hsv(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

/*
	hsv_cli_exec_update_rgb is triggered when command to be executed is 'rgb'.
	Requires 3 arguments:
		1. Red - range from 0 to 255.
		2. Green - range from 0 to 255.
		3. Blue - range from 0 to 255.
*/
void hsv_cli_exec_update_rgb(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

/*
	hsv_cli_exec_add_rgb_color is triggered when command to be executed is 'add_rgb_color'.
	Saves color to the buffer and flash.
	Requires 4 arguments:
		1. Red - range from 0 to 255.
		2. Green - range from 0 to 255.
		3. Blue - range from 0 to 255.
		4. Color name - name of a color to be saved
*/
void hsv_cli_exec_add_rgb_color(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

/*
	hsv_cli_exec_apply_color is triggered when command to be executed is 'apply_color'.
	Makes rgb color to display specified saved color.
	Requires 1 argument:
		1. Color name - name of a color to be displayed.
*/
void hsv_cli_exec_apply_color(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

/*
	hsv_cli_exec_del_color is triggered when command to be executed is 'del_color'.
	Deletes specified color from the list of saved colors.
	Requires 1 argument:
		1. Color name - name of a color to be deleted.
*/
void hsv_cli_exec_del_color(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

/*
	hsv_cli_exec_add_curr_color is triggered when command to be executed is 'add_current_color'.
	Saves currently displayed color under specified name.
	Requires 1 argument:
		1. Color name - name which will be given to a currently displayed color.
*/
void hsv_cli_exec_add_curr_color(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

#endif