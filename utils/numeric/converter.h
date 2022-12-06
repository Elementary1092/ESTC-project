#ifndef ESTC_UTILS_NUMERIC_CONVERTER_H
#define ESTC_UTILS_NUMERIC_CONVERTER_H

#include <stdint.h>

/*
	utils_numeric_converter_f_to_u16 tries to convert float { value } to uint16_t.
	Returns:
		- { max_value } - if UINT16_MAX < { value }, or { value } > { max_value }, or { value } = NaN.
		- 0 - if { value } < 0
		- converted value - otherwise
*/
uint16_t utils_numeric_converter_f_to_u16(float value, uint16_t max_value);

#endif