#ifndef ESTC_STRING_UTILS_H
#define ESTC_STRING_UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "app_config.h"

#define UTILS_STRINGS_MAX_STR_SIZE ESTC_MAX_LINE_SIZE

uint32_t utils_strings_atou(char *str);

size_t utils_strings_tokenize(char buf[][UTILS_STRINGS_MAX_STR_SIZE], const char *str, const char *delims);

bool utils_strings_does_contain(const char *str, const char ch);

#endif