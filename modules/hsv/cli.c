#include <nrf_log.h>
#include <app_usbd_cdc_acm.h>

#include "cli.h"
#include "hsv_picker.h"

#include "../cdc_acm/cdc_acm.h"

#define HSV_CLI_MAX_WORD_SIZE 4

#define HSV_CLI_UNKNOWN_COMMAND_PROMPT "Unknown command. Try again.\n"

static char word_buf[HSV_CLI_MAX_WORD_SIZE] = {0};

static char args_buf[HSV_CLI_MAX_ARGS_SIZE][HSV_CLI_MAX_WORD_SIZE];

static hsv_cli_command_desc_t curr_command;

static void hsv_cli_usb_evt_handler(app_usbd_class_inst_t const *p_inst,
									app_usbd_cdc_acm_user_event_t event);

static void hsv_cli_clear_command(hsv_cli_command_desc_t *command);

static void hsv_cli_resolve_command(hsv_cli_command_desc_t *command,
									const char *word, 
									ssize_t word_size);

static void hsv_cli_exec_command(hsv_cli_command_desc_t *command);

static void hsv_cli_exec_help(hsv_cli_command_desc_t *command);

static void hsv_cli_exec_update_hsv(hsv_cli_command_desc_t *command);

static void hsv_cli_exec_update_rgb(hsv_cli_command_desc_t *command);

APP_USBD_CDC_ACM_GLOBAL_DEF(hsv_cli_usb_cdc_acm,
							hsv_cli_usb_evt_handler,
							HSV_CLI_CDC_ACM_COMM_INTERFACE,
							HSV_CLI_CDC_ACM_DATA_INTERFACE,
							HSV_CLI_CDC_ACM_COMM_EPIN,
							HSV_CLI_CDC_ACM_DATA_EPIN,
							HSV_CLI_CDC_ACM_DATA_EPOUT,
							APP_USBD_CDC_COMM_PROTOCOL_NONE);

static void hsv_cli_usb_evt_handler(app_usbd_class_inst_t const *p_inst,
									app_usbd_cdc_acm_user_event_t event)
{
	switch(event)
	{
		case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
		{
			ssize_t read_count = cdc_acm_echo(&hsv_cli_usb_cdc_acm, word_buf, HSV_CLI_MAX_WORD_SIZE);
			hsv_cli_resolve_command(&curr_command, word_buf, read_count);
			hsv_cli_exec_command(&curr_command);
			break;	
		}

		case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
		{
			ssize_t read_count = cdc_acm_echo(&hsv_cli_usb_cdc_acm, word_buf, HSV_CLI_MAX_WORD_SIZE);
			hsv_cli_resolve_command(&curr_command, word_buf, read_count);
			hsv_cli_exec_command(&curr_command);
			break;
		}

		default:
			break;
	}
}

static void hsv_cli_resolve_command(hsv_cli_command_desc_t *command,
									const char *word, 
									ssize_t word_size)
{
	if (command->cmd_resolved)
	{
		strcpy(command->args[command->args_count], word);
		(command->args_count)++;
		return;
	}

	command->cmd_resolved = true;
	if (strcmp(word, HSV_CLI_COMMAND_HELP_STR) == 0)
	{
		command->cmd = HSV_CLI_COMMAND_HELP;
	}
	else if (strcmp(word, HSV_CLI_COMMAND_HSV_STR) == 0)
	{
		command->cmd = HSV_CLI_COMMAND_UPDATE_HSV;
	}
	else if (strcmp(word, HSV_CLI_COMMAND_RGB_STR) == 0)
	{
		command->cmd = HSV_CLI_COMMAND_UPDATE_RGB;
	}
	else
	{
		command->cmd = HSV_CLI_COMMAND_UNKNOWN;
	}
}

static void hsv_cli_exec_command(hsv_cli_command_desc_t *cmd)
{
	switch (cmd->cmd)
	{
		case HSV_CLI_COMMAND_UNKNOWN:
			cdc_acm_write(&hsv_cli_usb_cdc_acm, HSV_CLI_UNKNOWN_COMMAND_PROMPT, strlen(HSV_CLI_UNKNOWN_COMMAND_PROMPT));
			break;

		case HSV_CLI_COMMAND_HELP:
			hsv_cli_exec_help(cmd);
			break;

		case HSV_CLI_COMMAND_UPDATE_HSV:
			hsv_cli_exec_update_hsv(cmd);
			break;

		case HSV_CLI_COMMAND_UPDATE_RGB:
			hsv_cli_exec_update_rgb(cmd);
			break;

		default:
#if NRF_LOG_ENABLED
			NRF_LOG_ERROR("hsv_cli_exec_command: Unknown command: %d", cmd->cmd);
#endif
			break;
	}
}

static void hsv_cli_clear_command(hsv_cli_command_desc_t *command)
{
	command->cmd = HSV_CLI_COMMAND_UNKNOWN;
	command->cmd_resolved = false;
	memset(args_buf, 0, sizeof(args_buf));
	command->args = args_buf;
	command->args_count = 0;
	command->is_finished = false;
}

void hsv_cli_init(void)
{
	cdc_acm_init(&hsv_cli_usb_cdc_acm);
	hsv_cli_clear_command(&curr_command);
}
