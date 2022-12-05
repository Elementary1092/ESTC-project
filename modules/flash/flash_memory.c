#include <stdbool.h>
#include <nrfx_nvmc.h>
#include <nrf_log.h>
#include "app_config.h"
#include "flash_memory.h"

#define FLASH_MEMORY_DEFAULT_VALUE         0xFFFFFFFF

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

uint32_t flash_memory_seek_page_first_free_addr(uint32_t page_addr)
{
	if (page_addr % FLASH_MEMORY_PAGE_SIZE != 0)
	{
		return 0U;
	}

	uint32_t const *paddr = (uint32_t *)page_addr;

	uint32_t start_offset = 0U;
	uint32_t end_offset = FLASH_MEMORY_PAGE_SIZE / sizeof(uint32_t) - 1;
	
	if (*(paddr+end_offset) != FLASH_MEMORY_DEFAULT_VALUE)
	{
		return (uint32_t)(paddr + end_offset + 1);
	}

	while (start_offset < end_offset)
	{
		uint32_t mid_offset = end_offset - (end_offset - start_offset) / 2;
		if (*(paddr + mid_offset) != FLASH_MEMORY_DEFAULT_VALUE)
		{
			start_offset = mid_offset + 1;
		}
		else
		{
			end_offset = mid_offset - 1;
		}
	}

	if (*(paddr + start_offset) != FLASH_MEMORY_DEFAULT_VALUE)
	{
		return (uint32_t)(paddr + start_offset + 1);
	}
	else if (*(paddr + end_offset) != FLASH_MEMORY_DEFAULT_VALUE)
	{
		return (uint32_t)(paddr + end_offset + 1);
	}
	else
	{
		return 0U;
	}
}

flash_memory_err_t flash_memory_page_append(uint32_t *buffer, 
											uint32_t buf_size, 
											uint32_t page_addr, 
											uint32_t control_w,
											flash_memory_flag_t flags)
{
	if (page_addr % FLASH_MEMORY_PAGE_SIZE != 0)
	{
		return FLASH_MEMORY_ERR_INVALID_PAGE_ADDR;
	}

	bool can_erase_page = (flags & FLASH_MEMORY_ERASE_PAGE_IF_NECESSARY) != 0;
	bool ignore_control_w = (flags & FLASH_MEMORY_IGNORE_CONTROL_W) != 0;
	bool no_flags = (flags & FLASH_MEMORY_NO_FLAGS) != 0;
	if (no_flags)
	{
		ignore_control_w = false;
		can_erase_page = false;
	}

	uint32_t s_addr = flash_memory_seek_page_first_free_addr(page_addr);
	if (s_addr == 0U)
	{
		if (can_erase_page)
		{
			NRF_LOG_INFO("flash_memory_page_append: Erasing page");
			nrf_nvmc_page_erase(page_addr);
			s_addr = page_addr;
		}
		else
		{
			return FLASH_MEMORY_ERR_NOT_ENOUGH_SPACE;
		}
	}
	
	if (!ignore_control_w)
	{
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
}
