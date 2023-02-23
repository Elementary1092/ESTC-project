#include <string.h>
#include "cli_described_result.h"

void estc_cli_describe_error(estc_cli_error_t error, char buffer[512])
{
	char * error_description = estc_cli_errors_stringify(error);
	memset(buffer, '\0', 512);
	memcpy(buffer, error_description, 511);
}