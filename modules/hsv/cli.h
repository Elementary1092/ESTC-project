#ifndef HSV_CLI_H
#define HSV_CLI_H

#include <stdint.h>
#include <stdbool.h>
#include "app_config.h"

#ifndef HSV_CLI_CDC_ACM_COMM_EPIN
#define HSV_CLI_CDC_ACM_COMM_EPIN NRFX_USBD_EPIN3
#endif

#ifndef HSV_CLI_CDC_ACM_COMM_INTERFACE
#define HSV_CLI_CDC_ACM_COMM_INTERFACE 2
#endif

#ifndef HSV_CLI_CDC_ACM_DATA_INTERFACE
#define HSV_CLI_CDC_ACM_DATA_INTERFACE 3
#endif

#ifndef HSV_CLI_CDC_ACM_DATA_EPIN
#define HSV_CLI_CDC_ACM_DATA_EPIN NRFX_USBD_EPIN4
#endif

#ifndef HSV_CLI_CDC_ACM_DATA_EPOUT
#define HSV_CLI_CDC_ACM_DATA_EPOUT NRFX_USBD_EPOUT4
#endif

#define HSV_CLI_MAX_ARGS_SIZE 3

#define HSV_CLI_COMMAND_HELP_STR "help"

#define HSV_CLI_COMMAND_HSV_STR  "hsv"

#define HSV_CLI_COMMAND_RGB_STR  "rgb"

typedef enum
{
	HSV_CLI_COMMAND_UNKNOWN,
	HSV_CLI_COMMAND_HELP,
	HSV_CLI_COMMAND_UPDATE_HSV,
	HSV_CLI_COMMAND_UPDATE_RGB
} hsv_cli_command_t;

typedef struct
{
	hsv_cli_command_t cmd;
	char              **args;
	uint8_t           args_count;
	bool              is_finished;
	bool              cmd_resolved;
} hsv_cli_command_desc_t;

void hsv_cli_init(void);

#endif // HSV_CLI_H