#ifndef ESTC_UTILS_NUMERIC_H
#define ESTC_UTILS_NUMERIC_H

/**
 * @brief Tries to return absolute value of a value.
 * 
 * @param[in] value Floating point number which should be converted to positive or 0.
 * 
 * @retval 0 If the value is NaN or is Infinity.
 * 
 * @retval absolute_value If the value is correct floating point number.
*/
float utils_numeric_ops_absf(float value);

/**
 * @brief Computes remainder of the operation value / mod_base.
 * 
 * @param[in] value Numerator in the division.
 * 
 * @param[in] mod_base Denominator in the division.
 * 
 * @retval 0 If the value is NaN or is Infinity.
 * 
 * @retval @ref value If the result of the division is NaN.
 * 
 * @retval correct_result otherwise.
*/
float utils_numeric_ops_modf(float value, float mod_base);

#endif