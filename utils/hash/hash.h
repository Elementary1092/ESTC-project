#ifndef ESTC_UTILS_HASH_H
#define ESTC_UTILS_HASH_H

#include <stdint.h>

/* Implementation of Jenkins one at time hashing function */
uint32_t utils_hash_str_jenkins(const char *str);

#endif