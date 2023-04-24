#include <stdbool.h>
#include <nrf_soc.h>
#include <nrfx_nvmc.h>
#include <nrf_log.h>
#include <app_error.h>
#include "flash_memory.h"

static void flash_memory_wait_for_write_complition(void)
{
	while (!nrfx_nvmc_write_done_check());
}

void flash_memory_init(void)
{
	
}

flash_memory_err_t flash_memory_read(uint32_t addr,
									uint32_t *buffer,
									uint32_t limit, 
									uint32_t const *control_w,
									flash_memory_flag_t flags)
{
	bool ignore_control_w = (flags & FLASH_MEMORY_IGNORE_CONTROL_W) != 0;
	bool ignore_default_values = (flags & FLASH_MEMORY_IGNORE_DEFAULT_VALUES) != 0;
	bool no_flags = (flags & FLASH_MEMORY_NO_FLAGS) != 0;

	if (no_flags)
	{
		NRF_LOG_INFO("%s:%s:%d: Ignoring all flags.", NRF_LOG_PUSH(__FILE__), __func__, __LINE__);

		ignore_control_w = false;
		ignore_default_values = false;
	}

	uint32_t *s_addr = (uint32_t *)(addr);

	NRF_LOG_INFO("%s:%s:%d: Reading from %ld", NRF_LOG_PUSH(__FILE__), __func__, __LINE__, addr);

	if (!ignore_control_w)
	{
		NRF_LOG_INFO("%s:%s:%d: Checking control word.", NRF_LOG_PUSH(__FILE__), __func__, __LINE__);

		uint32_t word = *s_addr;
		if (word != *control_w)
		{
			NRF_LOG_ERROR("%s:%s:%d: Control word expected: %ld, got: %ld", NRF_LOG_PUSH(__FILE__), __func__, __LINE__, *control_w, word);
			return FLASH_MEMORY_ERR_INVALID_CONTROL_W;
		}
		s_addr += 1;
	}

	for (uint32_t i = 0; i < limit; i++)
	{
		uint32_t word = s_addr[i];

		NRF_LOG_INFO("%s:%s:%d: Read %x from %lu.", NRF_LOG_PUSH(__FILE__), __func__, __LINE__, word, s_addr + i);
		if (!ignore_default_values && word == FLASH_MEMORY_DEFAULT_VALUE)
		{
			return FLASH_MEMORY_ERR_POSSIBLY_INVALID_DATA;
		}

		buffer[i] = word;
	}

	return FLASH_MEMORY_NO_ERR;
}

flash_memory_err_t flash_memory_write(uint32_t addr,
									uint32_t *buffer, 
									uint32_t buf_size, 
									uint32_t const *control_w, 
									flash_memory_flag_t flags)
{
	bool ignore_control_w = (flags & FLASH_MEMORY_IGNORE_CONTROL_W) != 0;
	bool should_erase_page = (flags & FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE) != 0;
	bool no_flags = (flags & FLASH_MEMORY_NO_FLAGS) != 0;

	if (no_flags)
	{
		NRF_LOG_INFO("%s:%d: Ignoring all flags.", __func__, __LINE__);

		ignore_control_w = false;
		should_erase_page = false;
	}

	if (should_erase_page)
	{
		uint32_t p_addr = (addr / FLASH_MEMORY_PAGE_SIZE) * FLASH_MEMORY_PAGE_SIZE;
		NRF_LOG_INFO("%s:%d: Erasing page starting from address: %lu", __func__, __LINE__, p_addr);
		nrf_nvmc_page_erase(p_addr);
	}
	
	if (addr < FLASH_MEMORY_DATA_STARTING_ADDRESS || addr > FLASH_MEMORY_DATA_END_ADDRESS)
	{
		return FLASH_MEMORY_ERR_ADDR_OUT_OF_BOUND;
	}

	NRF_LOG_INFO("%s:%d: Start writing from %lu", __func__, __LINE__, addr);

	if (!ignore_control_w)
	{
		NRF_LOG_INFO("%s:%d: Writing control word: %lu", __func__, __LINE__, *control_w);

		if (*control_w == FLASH_MEMORY_DEFAULT_VALUE)
		{
			return FLASH_MEMORY_ERR_INVALID_CONTROL_W;
		}

		if (!nrfx_nvmc_word_writable_check(addr, *control_w))
		{
			NRF_LOG_INFO("%s:%d: Could not write control word: %x. Current addr value: %lu", __func__, __LINE__, *control_w, *((uint32_t const *)addr));

			return FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE;
		}

		nrfx_nvmc_word_write(addr, *control_w);
		flash_memory_wait_for_write_complition();

		addr += sizeof(uint32_t);
	}

	for (uint32_t i = 0; i < buf_size; i++)
	{
		if (!nrfx_nvmc_word_writable_check(addr, buffer[i]))
		{
			return FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE;
		}
	}

	nrfx_nvmc_words_write(addr, (void *)buffer, buf_size);
	flash_memory_wait_for_write_complition();

	NRF_LOG_INFO("%s:%d: Successfully saved words", __func__, __LINE__);

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
		return 0U;
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
											uint32_t const *control_w,
											flash_memory_flag_t flags)
{
	if (page_addr % FLASH_MEMORY_PAGE_SIZE != 0)
	{
		return FLASH_MEMORY_ERR_INVALID_PAGE_ADDR;
	}

	bool can_erase_page = (flags & FLASH_MEMORY_ERASE_PAGE_IF_NECESSARY) != 0;
	bool no_flags = (flags & FLASH_MEMORY_NO_FLAGS) != 0;
	if (no_flags)
	{
		can_erase_page = false;
	}

	uint32_t s_addr = flash_memory_seek_page_first_free_addr(page_addr);
	if (s_addr == 0U || (s_addr + buf_size * sizeof(uint32_t)) > (page_addr + FLASH_MEMORY_PAGE_SIZE))
	{
		if (can_erase_page)
		{
			NRF_LOG_INFO("%s:%s:%d: Erasing page", NRF_LOG_PUSH(__FILE__), __func__, __LINE__);
			nrf_nvmc_page_erase(s_addr);
			s_addr = page_addr;
		}
		else
		{
			return FLASH_MEMORY_ERR_NOT_ENOUGH_SPACE;
		}
	}
	
	return flash_memory_write(s_addr, buffer, buf_size, control_w, flags);
}
