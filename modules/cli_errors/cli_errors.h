#ifndef ESTC_CLI_ERRORS_H
#define ESTC_CLI_ERRORS_H

#define ESTC_CLI_ERRORS_EXPAND_AS_ENUM(a, b) a,
#define ESTC_CLI_ERRORS_EXPAND_AS_ERROR_STRING(a, b) b,

#define ESTC_CLI_ERRORS_ENUM_TO_STRING_MAP(EXPANDER) \
        EXPANDER(ESTC_CLI_SUCCESS, "Successfully completed") \
		EXPANDER(ESTC_CLI_ERROR_INVALID_NUMBER_OF_ARGS, "Invalid number of arguments") \
		EXPANDER(ESTC_CLI_ERROR_BUFFER_OVERFLOW, "Already have maximum number of elements") \
		EXPANDER(ESTC_CLI_ERROR_NOT_FOUND, "Such element was not found") \
		EXPANDER(ESTC_CLI_ERROR_INTERNAL, "Internal error has occurred") \
		EXPANDER(ESTC_CLI_ERROR_FAILED_TO_SAVE, "Failed to save") \
		EXPANDER(ESTC_CLI_ERROR_FAILED_TO_READ_FROM_FLASH, "Failed to load from the flash memory") \
		EXPANDER(ESTC_CLI_ERROR_UNKNOWN_COMMAND, "Unknown command") \
		EXPANDER(ESTC_CLI_ERROR_EMPTY_COMMAND, "") \

/**
 * @brief Describe all possible errors which may be returned by CLI handlers.
*/
typedef enum
{
	ESTC_CLI_ERRORS_ENUM_TO_STRING_MAP(ESTC_CLI_ERRORS_EXPAND_AS_ENUM)
} estc_cli_error_t;

/**
 * @brief Get description of CLI error
 * 
 * @param[in] error CLI error
 * 
 * @return String description of CLI error
 * 
 * @retval "" on invalid CLI error
*/
char * estc_cli_errors_stringify(estc_cli_error_t error);

#endif