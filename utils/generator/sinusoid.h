#ifndef ESTC_UTILS_GENERATOR_SINUSOID_H
#define ESTC_UTILS_GENERATOR_SINUSOID_H

/**
 * @brief Parameters used to generate sinusoid.
*/
typedef struct
{
	float current_value;     /**< Currently generated value (should be valid after calling next()).*/
	float current_angle;     /**< Current angle used to generate sinusoid value. */
	float angle_step;        /**< Increment of decrement @ref current_angle after each call of next(). */
	float max_angle_value;   /**< Maximum angle of which may be passed to the sin() function. */
	float angle_factor;      /**< Controls speed of changing of the next value. */
} utils_generator_sinusoid_ctx_t;

/**
 * @brief Generates value by using sin() function.
 * 
 * @param[in] generator_ctx Values used to generate next value.
*/
void utils_generator_sinusoid_next(utils_generator_sinusoid_ctx_t *generator_ctx);

#endif