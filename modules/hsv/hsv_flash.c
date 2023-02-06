#include <stdbool.h>
#include <stdint.h>
#include <nrf_log.h>
#include "modules/flash/flash_memory.h"
#include "hsv_converter.h"

#define HSV_SAVED_VALUE_CONTROL_WORD 0xF0F0F0F0
#define HSV_SAVED_VALUE_OFFSET 0UL

#define HSV_SAVED_BUFFER_IDX_HUE 0
#define HSV_SAVED_BUFFER_IDX_SATUR 1
#define HSV_SAVED_BUFFER_IDX_BRIGHT 2

union float_uint32_u
{
	float fl;
	uint32_t ui;
};

hsv_ctx_t hsv_picker_flash_try_init(void)
{
	uint32_t buffer[3] = {0UL};
	uint32_t addr = flash_memory_seek_page_first_free_addr(FLASH_MEMORY_FIRST_PAGE);
	if (addr == 0U)
	{
		return (hsv_ctx_t) {
			.is_valid = false,
		};
	}

	addr -= sizeof(uint32_t) * 4;
	flash_memory_err_t err = flash_memory_read(
		addr,
		buffer,
		3UL,
		HSV_SAVED_VALUE_CONTROL_WORD,
		FLASH_MEMORY_NO_FLAGS);

	if (err != FLASH_MEMORY_NO_ERR)
	{
		NRF_LOG_INFO("%s: Could not initialize from the flash. Error: %s", __func__, NRF_LOG_ERROR_STRING_GET(err));
		return (hsv_ctx_t) {
			.is_valid = false,
		};
	}

	union float_uint32_u hue;
	hue.ui = buffer[HSV_SAVED_BUFFER_IDX_HUE];
	union float_uint32_u satur;
	satur.ui = buffer[HSV_SAVED_BUFFER_IDX_SATUR];
	union float_uint32_u bright;
	bright.ui = buffer[HSV_SAVED_BUFFER_IDX_BRIGHT];

	NRF_LOG_INFO("HSV values from the flash: %x, %x, %x.", hue.fl, satur.fl, bright.fl);

	return (hsv_ctx_t) {
		.hue = hue.fl,
		.brightness = bright.fl,
		.saturation = satur.fl,
		.is_valid = true,
	};
}

bool hsv_picker_flash_update_saved_value(hsv_ctx_t const *hsv)
{
	if (!hsv->is_valid) 
	{
		return false;
	}

	union float_uint32_u hue;
	hue.fl = hsv->hue;
	union float_uint32_u satur;
	satur.fl = hsv->saturation;
	union float_uint32_u bright;
	bright.fl = hsv->brightness;

	uint32_t buffer[3] = {hue.ui, satur.ui, bright.ui};
	flash_memory_err_t err = flash_memory_page_append(
		buffer,
		3UL,
		FLASH_MEMORY_FIRST_PAGE,
		HSV_SAVED_VALUE_CONTROL_WORD,
		FLASH_MEMORY_ERASE_PAGE_IF_NECESSARY);

	if (err != FLASH_MEMORY_NO_ERR)
	{
		NRF_LOG_INFO("hsv_picker_update_saved_value: Could not update saved value. Error: %d", err);
		return false;
	}

	return true;
}
