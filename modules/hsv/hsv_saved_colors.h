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
	HSV_SAVED_COLORS_SAVED_COLORS_EMPTY
} hsv_saved_colors_err_t;

void hsv_saved_colors_load(void);

hsv_saved_colors_err_t hsv_saved_colors_seek(hsv_saved_color_rgb_t *rgb, uint32_t seeking_hash);

hsv_saved_colors_err_t hsv_saved_colors_add_or_mod(hsv_saved_color_rgb_t *rgb);

hsv_saved_colors_err_t hsv_saved_colors_delete(uint32_t color_name_hash);

#endif