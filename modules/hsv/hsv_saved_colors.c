#include <stdint.h>
#include <sys/types.h>

#include "utils/hash/hash.h"
#include "hsv_saved_colors.h"

static uint32_t saved_colors[HSV_SAVED_COLORS_MAX_SLOTS][3] = {0U};
static uint32_t saved_color_name_hashes[HSV_SAVED_COLORS_MAX_SLOTS] = {0U};
static uint32_t saved_colors_idx = 0U;

void hsv_saved_colors_load(void)
{

}

hsv_saved_colors_err_t hsv_saved_colors_seek(hsv_saved_color_rgb_t *rgb, char *color_name)
{
	uint32_t color_name_hash = utils_hash_str_jenkins(color_name);
	for (uint32_t i = 0; i < saved_colors_idx; i++)
	{
		if (saved_color_name_hashes[i] == color_name_hash)
		{
			rgb->red = saved_colors[i][0];
			rgb->green = saved_colors[i][1];
			rgb->blue = saved_colors[i][2];

			return HSV_SAVED_COLORS_SUCCESS;
		}
	}

	return HSV_SAVED_COLORS_NO_SUCH_COLOR;
}

hsv_saved_colors_err_t hsv_saved_colors_add_or_mod(hsv_saved_color_rgb_t *rgb, char *color_name)
{
	uint32_t color_name_hash = utils_hash_str_jenkins(color_name);
	for (size_t i = 0UL; i < saved_colors_idx; i++)
	{
		if (saved_color_name_hashes[i] == color_name_hash)
		{
			saved_colors[saved_colors_idx][0] = rgb->red;
			saved_colors[saved_colors_idx][1] = rgb->green;
			saved_colors[saved_colors_idx][2] = rgb->blue;
			
			return HSV_SAVED_COLORS_SUCCESS;
		}
	}

	if (saved_colors_idx >= HSV_SAVED_COLORS_MAX_SLOTS)
	{
		return HSV_SAVED_COLORS_NO_COLOR_SLOT;
	}

	saved_color_name_hashes[saved_colors_idx] = color_name_hash;
	saved_colors[saved_colors_idx][0] = rgb->red;
	saved_colors[saved_colors_idx][1] = rgb->green;
	saved_colors[saved_colors_idx][2] = rgb->blue;
	saved_colors_idx++;

	return HSV_SAVED_COLORS_SUCCESS;
}

hsv_saved_colors_err_t hsv_saved_colors_delete(char *color_name)
{
	if (saved_colors_idx == 0)
	{
		return HSV_SAVED_COLORS_SAVED_COLORS_EMPTY;
	}

	uint32_t color_name_hash = utils_hash_str_jenkins(color_name);

	uint32_t color_idx = 0U;
	while (color_idx < saved_colors_idx \
			&& saved_color_name_hashes[color_idx] == color_name_hash)
	{
		color_idx++;
	}

	if (color_idx == saved_colors_idx)
	{
		return HSV_SAVED_COLORS_NO_SUCH_COLOR;
	}

	saved_colors_idx--;
	for (uint32_t i = color_idx; i < saved_colors_idx; i++)
	{
		for (uint32_t j = 0UL; j < 3UL; j++)
		{
			saved_colors[i][j] = saved_colors[i+1][j];
		}
		saved_color_name_hashes[i] = saved_color_name_hashes[i+1];
	}

	return HSV_SAVED_COLORS_SUCCESS;
}
