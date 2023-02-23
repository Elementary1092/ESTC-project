#ifndef ESTC_CLI_DESCRIBED_RESULT_H
#define ESTC_CLI_DESCRIBED_RESULT_H

#include "cli_errors.h"

typedef void ( * estc_cli_result_describer_t(estc_cli_error_t error, char buffer[512]) );

typedef struct
{
	estc_cli_described_result_t result_describer;
	estc_cli_error_t            error;
} estc_cli_described_result_t;

void estc_cli_describe_error(estc_cli_error_t error, char buffer[512]);

#endif