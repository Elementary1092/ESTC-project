#include <stdbool.h>
#include <nrfx_nvmc.h>
#include <nrf_log.h>
#include "app_config.h"
#include "flash_memory.h"

#ifndef NRF_DFU_APP_DATA_AREA_SIZE
#error NRF_DFU_APP_DATA_AREA_SIZE is not defined
#endif

#define FLASH_MEMORY_DATA_STARTING_ADDRESS 0xE0000 - NRF_DFU_APP_DATA_AREA_SIZE
#define FLASH_MEMORY_DEFAULT_VALUE         0xFFFFFFFF
#define FLASH_MEMORY_PAGE_SIZE             4096

static const uint32_t addr = (uint32_t)FLASH_MEMORY_DATA_STARTING_ADDRESS;

static void flash_memory_wait_for_write_complition(void)
{
	while (!nrfx_nvmc_write_done_check());
}

flash_memory_err_t flash_memory_read(uint32_t buffer[], uint32_t limit, uint32_t offset, uint32_t control_w, flash_memory_flag_t flags)
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

	NRF_LOG_INFO("flash_memory_read: Reading from %ld", s_addr);

	if (!ignore_control_w)
	{
		NRF_LOG_INFO("flash_memory_read: Checking control word.");

		uint32_t word = *s_addr;
		if (word != control_w)
		{
			NRF_LOG_ERROR("flash_memory_read: Control word expected: %ld, got: %ld", control_w, word);
			return FLASH_MEMORY_ERR_INVALID_CONTROL_W;
		}
		s_addr += 1;
	}

	for (uint32_t i = 0; i < limit; i++)
	{
		uint32_t word = s_addr[i];

		NRF_LOG_INFO("flash_memory_read: Read %x from %lu.", word, s_addr + i);
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
		uint32_t p_addr = ((addr + offset) / FLASH_MEMORY_PAGE_SIZE) * FLASH_MEMORY_PAGE_SIZE;
		NRF_LOG_INFO("flash_memory_write: Erasing page starting from address: %lu", p_addr);
		nrf_nvmc_page_erase(p_addr);
	}

	uint32_t s_addr = addr + (offset * sizeof(uint32_t));
	
	NRF_LOG_INFO("flash_memory_write: Start writing from %lu", s_addr);

	if (!ignore_control_w)
	{
		NRF_LOG_INFO("flash_memory_write: Writing control word: %lu", control_w);

		if (control_w == FLASH_MEMORY_DEFAULT_VALUE)
		{
			return FLASH_MEMORY_ERR_INVALID_CONTROL_W;
		}

		if (!nrfx_nvmc_word_writable_check(s_addr, control_w))
		{
			NRF_LOG_INFO("flash_memory_write: Could not write control word: %x. Current addr value: %x", control_w, *((uint32_t *)s_addr));

			return FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE;
		}

		nrfx_nvmc_word_write(s_addr, control_w);

		flash_memory_wait_for_write_complition();

		s_addr += sizeof(uint32_t);
	}

	for (uint32_t i = 0; i < buf_size; i++)
	{
		if (!nrfx_nvmc_word_writable_check(s_addr, buffer[i]))
		{
			return FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE;
		}

		nrfx_nvmc_word_write(s_addr, buffer[i]);

		flash_memory_wait_for_write_complition();

		NRF_LOG_INFO("flash_memory_write: Written %x to %lu", buffer[i], s_addr);
		s_addr += sizeof(uint32_t);
	}

	NRF_LOG_INFO("flash_memory_write: Successfully saved words");

	return FLASH_MEMORY_NO_ERR;
}
