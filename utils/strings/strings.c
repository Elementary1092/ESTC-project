#include <string.h>
#include "strings.h"

uint32_t utils_strings_atou(const char *str)
{
	char *str_end;
	uint32_t res = (uint32_t)(strtoul(str, &str_end, 10));
	if (str_end != str + strlen(str))
	{
		return 0U;
	}

	return res;
}

bool utils_strings_does_contain(const char *str, const char ch)
{
	for (size_t i = 0; i < strlen(str); i++)
	{
		if (ch == str[i])
		{
			return true;
		}
	}

	return false;
}

size_t utils_strings_tokenize(char buf[][UTILS_STRINGS_MAX_STR_SIZE], const char *str, const char *delims)
{
	size_t buf_str_idx = 0;
	size_t buf_str_char_idx = 0;

	for (size_t i = 0; i < strlen(str); i++)
	{
		if (!utils_strings_does_contain(delims, str[i]))
		{
			buf[buf_str_idx][buf_str_char_idx] = str[i];
			buf_str_char_idx++;
		}
		else
		{
			if (buf_str_char_idx != 0)
			{
				buf_str_idx++;
			}
			buf_str_char_idx = 0;
		}
	}

	return buf_str_idx;
}
