#include <nrf_log.h>
#include <app_usbd_cdc_acm.h>
#include <string.h>
#include <stdlib.h>

#include "cli.h"
#include "hsv_picker.h"

#include "../cdc_acm/cdc_acm.h"

#define HSV_CLI_MAX_ARGS      5
#define HSV_CLI_MAX_WORD_SIZE 64

#define HSV_CLI_UNKNOWN_COMMAND_PROMPT "Unknown command. Try again.\r\n"

static char word_buf[HSV_CLI_MAX_WORD_SIZE] = {'\0'};

static char args_buf[HSV_CLI_MAX_ARGS][HSV_CLI_MAX_WORD_SIZE] = {'\0'};

static hsv_cli_command_desc_t curr_command;

static cdc_acm_read_buf_ctx_t acm_buf_ctx;

static void hsv_cli_usb_evt_handler(app_usbd_class_inst_t const *p_inst,
									app_usbd_cdc_acm_user_event_t event);

static void hsv_cli_clear_command(hsv_cli_command_desc_t *command);

static void hsv_cli_resolve_command(hsv_cli_command_desc_t *command,
									cdc_acm_read_buf_ctx_t const *read_buf);

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

static void hsv_cli_handle_rx_done(void)
{
	cdc_acm_ret_code_t ret;
	do 
	{
		ret = cdc_acm_echo(&hsv_cli_usb_cdc_acm, &acm_buf_ctx);
		if (ret == CDC_ACM_READ_NEW_LINE)
		{
			hsv_cli_resolve_command(&curr_command, &acm_buf_ctx);
			hsv_cli_exec_command(&curr_command);
			hsv_cli_clear_command(&curr_command);
		}
	} 
	while(ret != CDC_ACM_ACTION_ERROR);
}

static void hsv_cli_usb_evt_handler(app_usbd_class_inst_t const *p_inst,
									app_usbd_cdc_acm_user_event_t event)
{
	switch(event)
	{
		case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
		{
			char buf[1];
			cdc_acm_read_buf_ctx_t temp_buf = {
				.buf = buf,
				.buf_size = 1,
			};
			cdc_acm_read(&hsv_cli_usb_cdc_acm, &temp_buf);
			break;
		}
		case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
		{
			hsv_cli_handle_rx_done();
			break;
		}

		default:
			break;
	}
}

static inline bool hsv_cli_is_char_is_delim(char ch, const char *delims)
{
	for (size_t i = 0; i < strlen(delims); i++)
	{
		if (ch == delims[i])
		{
			return true;
		}
	}

	return false;
}

static size_t hsv_cli_tokenize_string(char buf[][HSV_CLI_MAX_WORD_SIZE], const char *str, const char *delims)
{
	size_t buf_str_idx = 0;
	size_t buf_str_char_idx = 0;

	for (size_t i = 0; i < strlen(str); i++)
	{
		if (!hsv_cli_is_char_is_delim(str[i], delims))
		{
			buf[buf_str_idx][buf_str_char_idx] = str[i];
			buf_str_char_idx++;
		}
		else
		{
			if (buf_str_char_idx != 0)
			{
				buf_str_idx++;
			}
			buf_str_char_idx = 0;
		}
	}

	return buf_str_idx;
}

static void hsv_cli_resolve_command(hsv_cli_command_desc_t *command,
									cdc_acm_read_buf_ctx_t const *read_buf)
{
	char args[HSV_CLI_MAX_ARGS + 1][HSV_CLI_MAX_WORD_SIZE];
	size_t nargs = hsv_cli_tokenize_string(args, read_buf->buf, " \r\n\t\0");
	
	command->cmd = HSV_CLI_COMMAND_UNKNOWN;
	if (nargs == 0)
	{
		return;
	}

	if (strcmp(args[0], HSV_CLI_COMMAND_HELP_STR) == 0)
	{
		command->cmd = HSV_CLI_COMMAND_HELP;
	}
	else if (strcmp(args[0], HSV_CLI_COMMAND_HSV_STR) == 0)
	{
		command->cmd = HSV_CLI_COMMAND_UPDATE_HSV;
	}
	else if (strcmp(args[0], HSV_CLI_COMMAND_RGB_STR) == 0)
	{
		command->cmd = HSV_CLI_COMMAND_UPDATE_RGB;
	}

	for (size_t i = 1; i < nargs; i++)
	{
		strcpy(command->args[i-1], args[i]);
	}

	command->args_count = nargs - 1;
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
			NRF_LOG_ERROR("hsv_cli_exec_command: Unknown command: %d", cmd->cmd);
			break;
	}
}

static void hsv_cli_exec_help(hsv_cli_command_desc_t *command)
{
	const char *help_prompt = 
						"Available commands:\r\n"
						"\t1. help - get list of available commands;\r\n"
						"\t2. hsv <hue> <saturation> <brightness> - set hue, saturation, and brightness of RGB LEDs;\r\n"
						"\t3. rgb <red> <green> <blue> - set values of RGB LEDs.\r\n";

	cdc_acm_write(&hsv_cli_usb_cdc_acm, help_prompt, strlen(help_prompt));
}

static void hsv_cli_exec_update_hsv(hsv_cli_command_desc_t *command)
{
	if (command->args_count != 3)
	{
		NRF_LOG_ERROR("hsv_cli_exec_update_hsv: Invalid number of args: %u", command->args_count);
		return;
	}

	uint32_t hsv_args[3];
	hsv_cli_convert_nstrs_to_nuints(hsv_args, command->args, command->args_count);

	hsv_picker_set_hsv((float)hsv_args[0], (float)hsv_args[1], (float)hsv_args[2]);
	NRF_LOG_INFO("hsv_cli_exec_update_hsv: Updated hsv");
}

static void hsv_cli_exec_update_rgb(hsv_cli_command_desc_t *command)
{
	if (command->args_count != 3)
	{
		NRF_LOG_ERROR("hsv_cli_exec_update_hsv: Invalid number of args: %u", command->args_count);
		return;
	}

	uint32_t rgb_args[3];
	hsv_cli_convert_nstrs_to_nuints(rgb_args, command->args, command->args_count);

	hsv_picker_set_rgb(rgb_args[0], rgb_args[1], rgb_args[2]);
	NRF_LOG_INFO("hsv_cli_exec_update_rgb: Updated rgb");
}

static uint32_t hsv_cli_convert_str_to_uint(char *str)
{
	uint32_t res = 0;
	if (strlen(str) > 10)
	{
		NRF_LOG_ERROR("hsv_cli_convert_str_to_uint: String is too long: %s", str);
		return res;
	}
	for (size_t i = 0; i < strlen(str); i++)
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			res = res * 10U + (uint32_t)(str[i] - '0');
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
	memset(args_buf, '\0', sizeof(args_buf));
	command->args = (char **)args_buf;
	command->args_count = 0;
}

void hsv_cli_init(void)
{
	cdc_acm_init(&hsv_cli_usb_cdc_acm);
	hsv_cli_clear_command(&curr_command);
	cdc_acm_read_buf_ctx_init(&acm_buf_ctx, word_buf, HSV_CLI_MAX_WORD_SIZE);
}
