#ifndef ESTC_STRING_UTILS_H
#define ESTC_STRING_UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "app_config.h"

#define UTILS_STRINGS_MAX_STR_SIZE ESTC_MAX_LINE_SIZE

/*
	utils_strings_atou parses uint32_t from { str }.
	Returns:
		- 0 - if value { str } is incorrect number or lenght of a { str } > 10 (otherwise will face overflow).
		- parsed value - otherwise.
*/
uint32_t utils_strings_atou(char *str);

/*
	utils_strings_tokenize divides string by specified delimiters and parses tokens to the buf.
	Returns:
		- number of parsed tokens.
*/
size_t utils_strings_tokenize(char buf[][UTILS_STRINGS_MAX_STR_SIZE], const char *str, const char *delims);

/*
	utils_strings_does_contain searches for a { ch } in { str }.
	Returns:
		- true - if { ch } is in { str }.
		- false - otherwise.
*/
bool utils_strings_does_contain(const char *str, const char ch);

#endif