#include <stdbool.h>
#include <nrfx_nvmc.h>
#include <nrf_bootloader_info.h>
#include <nrf_log.h>
#include "app_config.h"
#include "flash_memory.h"

#ifndef NRF_DFU_APP_DATA_AREA_SIZE
#error NRF_DFU_APP_DATA_AREA_SIZE is not defined
#endif

#define FLASH_MEMORY_DATA_STARTING_ADDRESS BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE
#define FLASH_MEMORY_DEFAULT_VALUE         0xFFFFFFFF
#define FLASH_MEMORY_PAGE_SIZE             4096

static const uint32_t addr = FLASH_MEMORY_DATA_STARTING_ADDRESS;

flash_memory_err_t flash_memory_read(uint32_t *buffer, uint32_t limit, uint32_t offset, uint32_t control_w, flash_memory_flag_t flags)
{
	bool ignore_control_w = (flags & FLASH_MEMORY_IGNORE_CONTROL_W) != 0;
	bool ignore_default_values = (flags & FLASH_MEMORY_IGNORE_DEFAULT_VALUES) != 0;
	bool no_flags = (flags & FLASH_MEMORY_NO_FLAGS) != 0;

	if (no_flags)
	{
		NRF_LOG_INFO("flash_memory_read: Ignoring all flags.");

		ignore_control_w = false;
		ignore_default_values = false;
	}

	uint32_t *s_addr = (uint32_t *)(addr + (offset * sizeof(uint32_t)));

	if (!ignore_control_w)
	{
		NRF_LOG_INFO("flash_memory_read: Checking control word.");

		uint32_t word = s_addr[0];
		if (word != control_w)
		{
			return FLASH_MEMORY_ERR_INVALID_CONTROL_W;
		}
		s_addr += sizeof(uint32_t);
	}

	for (uint32_t i = 0; i < limit; i++)
	{
		uint32_t word = s_addr[i];
		if (!ignore_default_values && word == FLASH_MEMORY_DEFAULT_VALUE)
		{
			return FLASH_MEMORY_ERR_POSSIBLY_INVALID_DATA;
		}

		buffer[i] = word;
	}

	return FLASH_MEMORY_NO_ERR;
}

flash_memory_err_t flash_memory_write(uint32_t *buffer, uint32_t buf_size, uint32_t offset, uint32_t control_w, flash_memory_flag_t flags)
{
	bool ignore_control_w = (flags & FLASH_MEMORY_IGNORE_CONTROL_W) != 0;
	bool should_erase_page = (flags & FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE) != 0;
	bool no_flags = (flags & FLASH_MEMORY_NO_FLAGS) != 0;

	if (no_flags)
	{
		NRF_LOG_INFO("flash_memory_write: Ignoring all flags.");

		ignore_control_w = false;
		should_erase_page = false;
	}

	if (should_erase_page)
	{
		uint32_t p_addr = addr + ((addr + offset) / FLASH_MEMORY_PAGE_SIZE) * FLASH_MEMORY_PAGE_SIZE;
		NRF_LOG_INFO("flash_memory_write: Erasing page starting from address: %ld", p_addr);
		NRFX_ASSERT(nrfx_nvmc_page_erase(p_addr));
	}

	uint32_t s_addr = addr + (offset * sizeof(uint32_t));

	if (!ignore_control_w)
	{
		NRF_LOG_INFO("flash_memory_write: Writing control word");

		if (control_w == FLASH_MEMORY_DEFAULT_VALUE)
		{
			return FLASH_MEMORY_ERR_INVALID_CONTROL_W;
		}

		if (nrfx_nvmc_word_writable_check(s_addr, control_w))
		{
			NRF_LOG_INFO("flash_memory_write: Could not write control word.")

			return FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE;
		}

		nrfx_nvmc_word_write(s_addr, control_w);
		s_addr += sizeof(uint32_t);
	}

	for (uint32_t i = 0; i < buf_size; i++)
	{
		if (nrfx_nvmc_word_writable_check(s_addr, buffer[i]))
		{
			return FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE;
		}

		nrfx_nvmc_word_write(s_addr, buffer[i]);
		s_addr += sizeof(uint32_t);
	}

	return FLASH_MEMORY_NO_ERR;
}
