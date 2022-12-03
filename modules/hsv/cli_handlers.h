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
		HANDLER(HSV_CLI_CMD_APPLY_COLOR, "apply_color", hsv_cli_exec_apply_color)

typedef enum
{
	HSV_CLI_HANDLERS(HSV_CLI_HANDLERS_EXPAND_AS_ENUM)
	HSV_CLI_COMMAND_END,
} hsv_cli_command_t;

typedef void (* hsv_cli_command_handler_t)(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

char *hsv_cli_get_cmd_str(hsv_cli_command_t h);
void hsv_cli_exec_handler(hsv_cli_command_t h, 
						  app_usbd_cdc_acm_t const *cdc_acm,
						  char args[][HSV_CLI_MAX_WORD_SIZE], 
						  uint8_t nargs);

void hsv_cli_exec_unknown(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);
void hsv_cli_exec_help(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);
void hsv_cli_exec_update_hsv(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);
void hsv_cli_exec_update_rgb(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);
void hsv_cli_exec_add_rgb_color(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);
void hsv_cli_exec_apply_color(app_usbd_cdc_acm_t const *cdc_acm, char args[][HSV_CLI_MAX_WORD_SIZE], uint8_t nargs);

#endif