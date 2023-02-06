#ifndef ESTC_STRING_UTILS_H
#define ESTC_STRING_UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "app_config.h"

#define UTILS_STRINGS_MAX_STR_SIZE ESTC_MAX_LINE_SIZE

/**
 * @brief Tries to parse uint32_t from the string.
 * 
 * @param[in] str String which should be converted.
 * 
 * @retval 0 If the provided string is invalid uint32_t number.
 * 
 * @retval parsed_value otherwise.
*/
uint32_t utils_strings_atou(const char *str);

/**
 * @brief Divides string by specified delimiters and parses tokens to the buf.
 * 
 * @details Scans copies characters of the string to the buffer 
 *          and if scanned character is in delimeters string starts parsing new token to the buffer.
 * 
 * @param[out] buf Buffer where all parsed tokens will be stored.
 * 
 * @param[in] str String which should be tokenized.
 * 
 * @param[in] delims Delimiters which specify the end of one token.
 * 
 * @return Number of parsed tokens.
*/
size_t utils_strings_tokenize(char buf[][UTILS_STRINGS_MAX_STR_SIZE], const char *str, const char *delims);

/**
 * @brief Searches for a character in the string.
 * 
 * @param[in] str The string where the character should be searched.
 * 
 * @param[in] ch The character which should be searched for in the string.
 * 
 * @retval true if the character is in the string.
 * 
 * @retval false if the character is not in the string.
*/
bool utils_strings_does_contain(const char *str, const char ch);

#endif