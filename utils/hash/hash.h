#ifndef ESTC_UTILS_HASH_H
#define ESTC_UTILS_HASH_H

#include <stdint.h>

/**
 * @brief Implementation of Jenkins one at time hashing function.
 * 
 * @retval uint32_t The hash of the string.
*/
uint32_t utils_hash_str_jenkins(const char *str);

#endif