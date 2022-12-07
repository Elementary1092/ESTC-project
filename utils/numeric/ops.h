#ifndef ESTC_UTILS_NUMERIC_H
#define ESTC_UTILS_NUMERIC_H

/*
	utils_numeric_ops_modf tries to return absolute value of a { value }.
	Returns:
		- 0 - if { value } is NaN.
		- absolute value - otherwise.
*/
float utils_numeric_ops_absf(float value);

/*
	utils_numeric_ops_modf performs modulo division operation on { value } where base = { mod_base }.
	Returns:
		- 0 - if { value } is NaN.
		- value - if result of modulo division is NaN.
		- modulo division result - otherwise.
*/
float utils_numeric_ops_modf(float value, float mod_base);

#endif