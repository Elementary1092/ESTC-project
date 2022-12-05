#include <stdint.h>
#include <sys/types.h>

#include "utils/hash/hash.h"
#include "modules/flash/flash_memory.h"
#include "hsv_saved_colors.h"

#define HSV_SAVED_COLOR_FLASH_BUF_SIZE     sizeof(hsv_saved_color_rgb_t) / sizeof(uint32_t) * HSV_SAVED_COLORS_MAX_SLOTS

#define HSV_SAVED_COLOR_FLASH_BUF_HASH(x)  sizeof(uint32_t) * (x)
#define HSV_SAVED_COLOR_FLASH_BUF_RED(x)   sizeof(uint32_t) * (x) + 1
#define HSV_SAVED_COLOR_FLASH_BUF_GREEN(x) sizeof(uint32_t) * (x) + 2
#define HSV_SAVED_COLOR_FLASH_BUF_BLUE(x)  sizeof(uint32_t) * (x) + 3

static hsv_saved_color_rgb_t saved_colors[HSV_SAVED_COLORS_MAX_SLOTS] = {0U};
static uint32_t saved_colors_size = 0U;

static void hsv_saved_colors_flush(void)
{
	uint32_t colors_to_mem[HSV_SAVED_COLOR_FLASH_BUF_SIZE] = {0U};
	for (uint32_t i = 0; i < saved_colors_size; i++)
	{
		colors_to_mem[HSV_SAVED_COLOR_FLASH_BUF_HASH(saved_colors_size)] = saved_colors[i].hash;
		colors_to_mem[HSV_SAVED_COLOR_FLASH_BUF_RED(saved_colors_size)] = saved_colors[i].red;
		colors_to_mem[HSV_SAVED_COLOR_FLASH_BUF_GREEN(saved_colors_size)] = saved_colors[i].green;
		colors_to_mem[HSV_SAVED_COLOR_FLASH_BUF_BLUE(saved_colors_size)] = saved_colors[i].blue;
	}

	flash_memory_write(
		FLASH_MEMORY_SECOND_PAGE,
		colors_to_mem,
		sizeof(hsv_saved_color_rgb_t) / sizeof(uint32_t) * saved_colors_size,
		0U,
		FLASH_MEMORY_IGNORE_CONTROL_W | FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE
	);
}

void hsv_saved_colors_load(void)
{
	uint32_t colors_from_mem[HSV_SAVED_COLOR_FLASH_BUF_SIZE] = {0U};
	flash_memory_read(FLASH_MEMORY_SECOND_PAGE, 
					colors_from_mem, 
					sizeof(colors_from_mem), 
					0U, 
					FLASH_MEMORY_IGNORE_CONTROL_W);
	for (saved_colors_size = 0U; saved_colors_size < HSV_SAVED_COLORS_MAX_SLOTS; saved_colors_size++)
	{
		uint32_t hash = colors_from_mem[HSV_SAVED_COLOR_FLASH_BUF_HASH(saved_colors_size)];
		uint32_t red = colors_from_mem[HSV_SAVED_COLOR_FLASH_BUF_RED(saved_colors_size)];
		uint32_t green = colors_from_mem[HSV_SAVED_COLOR_FLASH_BUF_GREEN(saved_colors_size)];
		uint32_t blue = colors_from_mem[HSV_SAVED_COLOR_FLASH_BUF_BLUE(saved_colors_size)];

		if (hash == 0U && red == 0U && green == 0U && blue == 0U)
		{
			break;
		}

		hsv_saved_color_rgb_t color = {
			.hash = hash,
			.red = red,
			.green = green,
			.blue = blue
		};
		saved_colors[saved_colors_size] = color;
	}
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

			hsv_saved_colors_flush();

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

	hsv_saved_colors_flush();

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

	hsv_saved_colors_flush();

	return HSV_SAVED_COLORS_SUCCESS;
}
