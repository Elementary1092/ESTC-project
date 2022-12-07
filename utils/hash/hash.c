#include <sys/types.h>
#include <string.h>
#include "hash.h"

uint32_t utils_hash_str_jenkins(const char *str)
{
	uint32_t hash = 0U;
	for (size_t i = 0UL; i < strlen(str); i++)
	{
		hash += (uint32_t)str[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	
	return hash;
}