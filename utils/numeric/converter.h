#ifndef ESTC_UTILS_NUMERIC_CONVERTER_H
#define ESTC_UTILS_NUMERIC_CONVERTER_H

#include <stdint.h>

/**
 * @brief Tries to convert floating point value to a valid uint16_t.
 * 
 * @param[in] value Floating point value which should be converted to uint16_t.
 * 
 * @param[in] max_value Maximum expected resulting value.
 * 
 * @retval max_value If UINT16_MAX < @ref value, value > max_value, or value is NaN or is Infinity.
 * 
 * @retval 0 If value < 0.
 * 
 * @retval converted_value otherwise.
*/
uint16_t utils_numeric_converter_f_to_u16(float value, uint16_t max_value);

#endif