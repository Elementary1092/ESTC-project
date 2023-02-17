#include <string.h>
#include "cli_errors.h"

static char * cli_errors_descriptions[] = {
	ESTC_CLI_ERRORS_ENUM_TO_STRING_MAP(ESTC_CLI_ERRORS_EXPAND_AS_ERROR_STRING)
};

char * estc_cli_errors_stringify(estc_cli_error_t error)
{
	if (error < 0 || ((size_t)error) >= sizeof(cli_errors_descriptions) / sizeof(cli_errors_descriptions[0]))
	{
		return "";
	}

	return cli_errors_descriptions[error];
}
