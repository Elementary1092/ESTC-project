#ifndef HSV_SAVED_COLORS_H
#define HSV_SAVED_COLORS_H

#ifndef HSV_SAVED_COLORS_MAX_SLOTS
#define HSV_SAVED_COLORS_MAX_SLOTS 10
#endif

typedef struct
{
	uint32_t hash;
	uint32_t red;
	uint32_t green;
	uint32_t blue;
} hsv_saved_color_rgb_t;

typedef enum
{
	HSV_SAVED_COLORS_SUCCESS,
	HSV_SAVED_COLORS_NO_SUCH_COLOR,
	HSV_SAVED_COLORS_NO_COLOR_SLOT,
	HSV_SAVED_COLORS_EMPTY
} hsv_saved_colors_err_t;

/*
	hsv_saved_colors_load tries to load saved colors from the flash memory.
*/
void hsv_saved_colors_load(void);

/*
	hsv_saved_color_seek searches for a color which hash of a name equals to { seeking_hash }
	and fills { rgb } with values of RGB of the saved color.

	Returns:
		- HSV_SAVED_COLORS_SUCCESS - if color is found;
		- HSV_SAVED_COLORS_NO_SUCH_COLOR - if could not find color;
*/
hsv_saved_colors_err_t hsv_saved_colors_seek(hsv_saved_color_rgb_t *rgb, uint32_t seeking_hash);

/*
	hsv_saved_colors_add_or_mod saves new color or modifies existing color (if color with { rgb }->hash already exists).
	Returns:
		- HSV_SAVED_COLORS_SUCCESS - if color was successfully saved.
		- HSV_SAVED_COLORS_NO_COLOR_SLOT - if there were already saved maximum number of colors.
*/
hsv_saved_colors_err_t hsv_saved_colors_add_or_mod(hsv_saved_color_rgb_t *rgb);

/*
	hsv_saved_colos_delete tries to delete color saved under name with hash = { color_name_hash }.
	Returns:
		- HSV_SAVED_COLORS_SUCCESS - if color was successfully deleted.
		- HSV_SAVED_COLORS_EMPTY - if no colors were saved before.
		- HSV_SAVED_COLORS_NO_SUCH_COLOR - if specified color was not found.
*/
hsv_saved_colors_err_t hsv_saved_colors_delete(uint32_t color_name_hash);

#endif