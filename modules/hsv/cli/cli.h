#ifndef HSV_CLI_H
#define HSV_CLI_H

#include <stdint.h>
#include <stdbool.h>
#include <app_usbd_cdc_acm.h>
#include "modules/cli_errors/cli_described_result.h"
#include "modules/cdc_acm/cdc_acm_read_buf.h"

/**
 * @brief Parses read buffer and executes command if a handler exists.
 * 
 * @details Resolves specified command (first word from the { read_buf } is treated as a command),
 *          and executes this command (all words except the first word from the 
 *          { read_buf } are treated as arguments of the command).
 * 
 * @param[in] read_buf Buffer from where command and its arguments may be parsed. 
*/
estc_cli_described_result_t hsv_cli_exec_command(cdc_acm_read_buf_t *read_buf);

#endif // HSV_CLI_H