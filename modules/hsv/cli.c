#include <nrf_log.h>
#include <app_usbd_cdc_acm.h>

#include "cli.h"
#include "hsv_picker.h"

#include "../cdc_acm/cdc_acm.h"

#define HSV_CLI_MAX_WORD_SIZE 4

#define HSV_CLI_UNKNOWN_COMMAND_PROMPT "Unknown command. Try again.\r\n"

static char word_buf[HSV_CLI_MAX_WORD_SIZE] = {0};

static char *args_buf[HSV_CLI_MAX_WORD_SIZE];

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

/* Invalid numbers are treated as 0 */
static uint32_t hsv_cli_convert_str_to_uint(char *str);

static void hsv_cli_convert_nstrs_to_nuints(uint32_t *converted_args, char **args, uint8_t args_count);

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

static void hsv_cli_exec_help(hsv_cli_command_desc_t *command)
{
	char *help_prompt = 
						"Available commands:\r\n"
						"\t1. help - get list of available commands;\r\n"
						"\t2. hsv <hue> <saturation> <brightness> - set hue, saturation, and brightness of RGB LEDs;\r\n"
						"\t3. rgb <red> <green> <blue> - set values of RGB LEDs.\r\n";

	cdc_acm_write(&hsv_cli_usb_cdc_acm, help_prompt, strlen(help_prompt));
}

static void hsv_cli_exec_update_hsv(hsv_cli_command_desc_t *command)
{
	if (!command->cmd_resolved || command->args_count < 3)
	{
		return;
	}
	else if (command->args_count > 3)
	{
#if NRF_LOG_ENABLED
		NRF_LOG_ERROR("hsv_cli_exec_update_hsv: Invalid number of args: %u", command->args_count);
#endif
		hsv_cli_clear_command(command);
		return;
	}

	uint32_t hsv_args[3];
	hsv_cli_convert_nstrs_to_nuints(hsv_args, command->args, command->args_count);

	hsv_picker_set_hsv((float)hsv_args[0], (float)hsv_args[1], (float)hsv_args[2]);
#if NRF_LOG_ENABLED
	NRF_LOG_INFO("hsv_cli_exec_update_hsv: Updated hsv");
#endif
}

static void hsv_cli_exec_update_rgb(hsv_cli_command_desc_t *command)
{
	if (!command->cmd_resolved || command->args_count < 3)
	{
		return;
	}
	else if (command->args_count > 3)
	{
#if NRF_LOG_ENABLED
		NRF_LOG_ERROR("hsv_cli_exec_update_hsv: Invalid number of args: %u", command->args_count);
#endif
		hsv_cli_clear_command(command);
		return;
	}

	uint32_t rgb_args[3];
	hsv_cli_convert_nstrs_to_nuints(rgb_args, command->args, command->args_count);

	hsv_picker_set_rgb(rgb_args[0], rgb_args[1], rgb_args[2]);
#if NRF_LOG_ENABLED
	NRF_LOG_INFO("hsv_cli_exec_update_rgb: Updated rgb");
#endif
}

static uint32_t hsv_cli_convert_str_to_uint(char *str)
{
	uint32_t res = 0;
	if (strlen(str) > 10)
	{
#if NRF_LOG_ENABLED
		NRF_LOG_ERROR("hsv_cli_convert_str_to_uint: String is too long: %s", str);
#endif
		return res;
	}
	for (size_t i = 0; i < strlen(str); i++)
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			res = res * 10 + (uint32_t)(str[i] - '0');
		}
		else
		{
			return 0;
		}
	}

	return res;
}

static void hsv_cli_convert_nstrs_to_nuints(uint32_t *converted_args, char **args, uint8_t args_count)
{
	for (uint8_t i = 0; i < args_count; i++)
	{
		converted_args[i] = hsv_cli_convert_str_to_uint(args[i]);
	}
}

static void hsv_cli_clear_command(hsv_cli_command_desc_t *command)
{
	command->cmd = HSV_CLI_COMMAND_UNKNOWN;
	command->cmd_resolved = false;
	memset(args_buf, 0, sizeof(args_buf));
	command->args = args_buf;
	command->args_count = 0;
}

void hsv_cli_init(void)
{
	cdc_acm_init(&hsv_cli_usb_cdc_acm);
	hsv_cli_clear_command(&curr_command);
}
