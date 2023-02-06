#ifndef HSV_SAVED_COLORS_H
#define HSV_SAVED_COLORS_H

#ifndef HSV_SAVED_COLORS_MAX_SLOTS
#define HSV_SAVED_COLORS_MAX_SLOTS 10
#endif

#include <stdint.h>

/**
 * @brief Describes a format of a color which should be saved.
*/
typedef struct
{
	uint32_t hash;
	uint32_t red;
	uint32_t green;
	uint32_t blue;
} hsv_saved_color_rgb_t;

/**
 * @brief Possible return value of functions of this module.
*/
typedef enum
{
	HSV_SAVED_COLORS_SUCCESS,
	HSV_SAVED_COLORS_NO_SUCH_COLOR,
	HSV_SAVED_COLORS_NO_COLOR_SLOT,
	HSV_SAVED_COLORS_EMPTY
} hsv_saved_colors_err_t;

/**
 * @brief Tries to load saved colors from the flash memory.
*/
void hsv_saved_colors_load(void);

/**
 * @brief Searches for a color.
 * 
 * @details Looks up a color which has the same hash as passed hash
 *          and fills rgb with values of RGB of the saved color.
 * 
 * @warning If rgb == NULL immediatelly returns.
 * 
 * @param[out] rgb Found color.
 * 
 * @param[in] seeking_hash A hash of a color name.
 * 
 * @retval @ref HSV_SAVED_COLORS_SUCCESS Color is found.
 * 
 * @retval @ref HSV_SAVED_COLORS_EMPTY Passed rgb == NULL.
 * 
 * @retval @ref HSV_SAVED_COLORS_NO_SUCH_COLOR Could not find color;
*/
hsv_saved_colors_err_t hsv_saved_colors_seek(hsv_saved_color_rgb_t *rgb, uint32_t seeking_hash);

/**
 * @brief Tries to save new color or modify an existing color (if color with rgb->hash already exists).
 * 
 * @warning If rgb == NULL immediatelly returns.
 * 
 * @warning Writing to the flash may fail but currently it is ignored. 
 *          So, some saved colors may be lost after session ends.
 * 
 * @param[in] rgb Color which should be saved.
 * 
 * @retval @ref HSV_SAVED_COLORS_SUCCESS Color was successfully saved.
 * 
 * @retval @ref HSV_SAVED_COLORS_NO_COLOR_SLOT There were already saved maximum number of colors.
 * 
 * @retval @ref HSV_SAVED_COLORS_EMPTY Passed rgb == NULL.
*/
hsv_saved_colors_err_t hsv_saved_colors_add_or_mod(hsv_saved_color_rgb_t const * rgb);

/**
 * @brief Tries to delete color saved under name with passed hash.
 * 
 * @param[in] color_name_hash A hash of a color name which should be deleted.
 * 
 * @retval @ref HSV_SAVED_COLORS_SUCCESS Color was successfully deleted.
 * 
 * @retval @ref HSV_SAVED_COLORS_EMPTY No colors were saved before.
 * 
 * @retval @ref HSV_SAVED_COLORS_NO_SUCH_COLOR Specified color was not found.
*/
hsv_saved_colors_err_t hsv_saved_colors_delete(uint32_t color_name_hash);

#endif