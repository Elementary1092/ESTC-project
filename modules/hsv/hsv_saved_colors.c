#include <stdint.h>
#include <sys/types.h>

#include "utils/hash/hash.h"
#include "modules/flash/flash_memory.h"
#include "hsv_saved_colors.h"

static hsv_saved_color_rgb_t saved_colors[HSV_SAVED_COLORS_MAX_SLOTS] = {0U};
static uint32_t saved_colors_size = 0U;

void hsv_saved_colors_load(void)
{
	
}

hsv_saved_colors_err_t hsv_saved_colors_seek(hsv_saved_color_rgb_t *rgb, uint32_t seeking_hash)
{
	for (uint32_t i = 0; i < saved_colors_size; i++)
	{
		if (saved_colors[i].hash == seeking_hash)
		{
			rgb->red = saved_colors[i].red;
			rgb->green = saved_colors[i].green;
			rgb->blue = saved_colors[i].blue;

			return HSV_SAVED_COLORS_SUCCESS;
		}
	}

	return HSV_SAVED_COLORS_NO_SUCH_COLOR;
}

hsv_saved_colors_err_t hsv_saved_colors_add_or_mod(hsv_saved_color_rgb_t *rgb)
{
	for (size_t i = 0UL; i < saved_colors_size; i++)
	{
		if (saved_colors[i].hash == rgb->hash)
		{
			saved_colors[saved_colors_size].red = rgb->red;
			saved_colors[saved_colors_size].green = rgb->green;
			saved_colors[saved_colors_size].blue = rgb->blue;

			return HSV_SAVED_COLORS_SUCCESS;
		}
	}

	if (saved_colors_size >= HSV_SAVED_COLORS_MAX_SLOTS)
	{
		return HSV_SAVED_COLORS_NO_COLOR_SLOT;
	}

	saved_colors[saved_colors_size].hash = rgb->hash;
	saved_colors[saved_colors_size].red = rgb->red;
	saved_colors[saved_colors_size].green = rgb->green;
	saved_colors[saved_colors_size].blue = rgb->blue;
	saved_colors_size++;

	return HSV_SAVED_COLORS_SUCCESS;
}

hsv_saved_colors_err_t hsv_saved_colors_delete(uint32_t color_name_hash)
{
	if (saved_colors_size == 0)
	{
		return HSV_SAVED_COLORS_SAVED_COLORS_EMPTY;
	}

	uint32_t color_idx = 0U;
	while (color_idx < saved_colors_size \
			&& saved_colors[color_idx].hash == color_name_hash)
	{
		color_idx++;
	}

	if (color_idx == saved_colors_size)
	{
		return HSV_SAVED_COLORS_NO_SUCH_COLOR;
	}

	saved_colors_size--;
	for (uint32_t i = color_idx; i < saved_colors_size; i++)
	{
		saved_colors[i].red = saved_colors[i + 1].red;
		saved_colors[i].green = saved_colors[i + 1].green;
		saved_colors[i].blue = saved_colors[i + 1].blue;
		saved_colors[i].hash = saved_colors[i + 1].hash;
	}

	return HSV_SAVED_COLORS_SUCCESS;
}
