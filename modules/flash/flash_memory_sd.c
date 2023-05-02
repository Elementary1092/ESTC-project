#include <nrf_fstorage.h>
#include <nrf_fstorage_sd.h>
#include <app_error.h>
#include <nrf_log.h>
#include <string.h>
#include <stdbool.h>
#include "flash_memory.h"

#define FLASH_MEMORY_SD_BUF_SIZE 4

static uint32_t write_buffer[FLASH_MEMORY_SD_BUF_SIZE][50];
static uint32_t write_addresses[FLASH_MEMORY_SD_BUF_SIZE];
static uint32_t write_lengths[FLASH_MEMORY_SD_BUF_SIZE];
static uint32_t buf_curr_idx;
static uint32_t buf_end_idx;
static bool is_buffer_full;

static void fstorage_evt_handler(nrf_fstorage_evt_t * evt);

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) = 
{
	.evt_handler = fstorage_evt_handler,
	.start_addr = FLASH_MEMORY_DATA_STARTING_ADDRESS,
	.end_addr = FLASH_MEMORY_DATA_END_ADDRESS,
};

struct flash_memory_flags_s
{
	unsigned int no_flags : 1;
	unsigned int ignore_control_w : 1;
	unsigned int ignore_default_values : 1;
	unsigned int erase_page_before_write : 1;
	unsigned int erase_if_necessary : 1;
};

static bool flash_memory_append_to_buffer(uint32_t addr, uint32_t num_words, uint32_t const * data)
{
	if (is_buffer_full)
	{
		return false;
	}

	write_addresses[buf_end_idx] = addr;
	write_lengths[buf_end_idx] = num_words * sizeof(uint32_t);
	memset((void *)(write_buffer[buf_end_idx]), 0, sizeof(write_buffer[0]));
	memcpy((void *)(write_buffer[buf_end_idx]), (const void *)data, num_words * sizeof(uint32_t));
	buf_end_idx = (buf_end_idx + 1) % FLASH_MEMORY_SD_BUF_SIZE;
	is_buffer_full = (buf_end_idx == buf_curr_idx);

	return true;
}

static bool flash_memory_write_or_bufferize(uint32_t addr, uint32_t num_words, uint32_t const * data)
{
	if (nrf_fstorage_is_busy(&fstorage))
	{
		return flash_memory_append_to_buffer(addr, num_words, data);
	}
	NRF_LOG_INFO("Writing to 0x%x (%u), data length: %u", addr, addr, num_words * sizeof(uint32_t));

	ret_code_t err_code = nrf_fstorage_write(&fstorage, addr, (const void *)data, num_words * sizeof(uint32_t), NULL);
	if (err_code != NRF_SUCCESS)
	{
		NRF_LOG_ERROR("%s:%d: Failed to write due to %s", __func__, __LINE__, NRF_LOG_ERROR_STRING_GET(err_code));
		return false;
	}

	return true;
}

static void fstorage_evt_handler(nrf_fstorage_evt_t * evt)
{
	NRF_LOG_INFO("Fstorage is handling event");
	if (evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
        return;
    }

	if (nrf_fstorage_is_busy(&fstorage))
	{
		return;
	}

	ret_code_t err_code = NRF_SUCCESS;

	if (buf_curr_idx != buf_end_idx)
	{
		NRF_LOG_INFO("Writing to 0x%x (%u), data length: %u", write_addresses[buf_curr_idx], write_addresses[buf_curr_idx], write_lengths[buf_curr_idx]);
		err_code = nrf_fstorage_write(
			&fstorage, 
			write_addresses[buf_curr_idx], 
			(const void *)(write_buffer[buf_curr_idx]), 
			write_lengths[buf_curr_idx], 
			NULL);
		if (err_code != NRF_SUCCESS)
		{
			NRF_LOG_ERROR(
				"%s:%d: Failed to write data to 0x%x (%u) due to %s", 
				__func__, 
				__LINE__, 
				write_addresses[buf_curr_idx], 
				write_addresses[buf_curr_idx], 
				NRF_LOG_ERROR_STRING_GET(err_code));
			return;
		}
		buf_curr_idx = (buf_curr_idx + 1) % FLASH_MEMORY_SD_BUF_SIZE;
		is_buffer_full = false;
	}
}

static struct flash_memory_flags_s flash_memory_parse_flags(flash_memory_flag_t flags)
{
	if (flags & FLASH_MEMORY_NO_FLAGS)
	{
		return (struct flash_memory_flags_s){
			.no_flags = 1,
		};
	}

	return (struct flash_memory_flags_s){
		.ignore_default_values = flags & FLASH_MEMORY_IGNORE_DEFAULT_VALUES ? 1 : 0,
		.ignore_control_w = flags & FLASH_MEMORY_IGNORE_CONTROL_W ? 1 : 0,
		.erase_if_necessary = flags & FLASH_MEMORY_ERASE_PAGE_IF_NECESSARY ? 1 : 0,
		.erase_page_before_write = flags & FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE ? 1 : 0,
	};
}

void flash_memory_init(void)
{
	nrf_fstorage_api_t * api;
	api = &nrf_fstorage_sd;

	ret_code_t err_code = nrf_fstorage_init(&fstorage, api, NULL);
	APP_ERROR_CHECK(err_code);
}

flash_memory_err_t flash_memory_read(uint32_t addr,
									uint32_t *buffer,
									uint32_t limit,
									uint32_t const *control_w, 
									flash_memory_flag_t flags)
{
	struct flash_memory_flags_s fls = flash_memory_parse_flags(flags);

	NRF_LOG_INFO("%s:%d: Reading from 0x%x (%u)", __func__, __LINE__, addr, addr);

	if (!fls.ignore_control_w)
	{
		NRF_LOG_INFO("%s:%d: Checking control word.", __func__, __LINE__);

		uint32_t word = 0U;
		ret_code_t err_code = nrf_fstorage_read(&fstorage, addr, &word, sizeof(uint32_t));
		if (err_code != NRF_SUCCESS)
		{
			NRF_LOG_ERROR(
				"%s:%d: Failed to read control word: %s", 
				__func__, 
				__LINE__, 
				NRF_LOG_ERROR_STRING_GET(err_code));
			return FLASH_MEMORY_ERR_FAILED_TO_READ;
		}

		if (word != *control_w)
		{
			NRF_LOG_ERROR("%s:%d: Control word expected: 0x%x, got: 0x%x", __func__, __LINE__, *control_w, word);
			return FLASH_MEMORY_ERR_INVALID_CONTROL_W;
		}
		addr += sizeof(*control_w);
	}

	ret_code_t err_code = nrf_fstorage_read(&fstorage, addr, buffer, limit * sizeof(buffer[0]));
	if (err_code != NRF_SUCCESS)
	{
		NRF_LOG_ERROR("%s:%d: Failed to read data: %s", __func__, __LINE__, NRF_LOG_ERROR_STRING_GET(err_code));
		return FLASH_MEMORY_ERR_FAILED_TO_READ;
	}

	return FLASH_MEMORY_NO_ERR;
}

flash_memory_err_t flash_memory_write(uint32_t addr,
									uint32_t *buffer, 
									uint32_t buf_size,
									uint32_t const *control_w, 
									flash_memory_flag_t flags)
{
	struct flash_memory_flags_s fls = flash_memory_parse_flags(flags);
	ret_code_t err_code = NRF_SUCCESS;

	if (fls.no_flags)
	{
		NRF_LOG_INFO("%s:%d: Ignoring all flags.", __func__, __LINE__);
	}
	
	if (addr < FLASH_MEMORY_DATA_STARTING_ADDRESS || addr > FLASH_MEMORY_DATA_END_ADDRESS)
	{
		return FLASH_MEMORY_ERR_ADDR_OUT_OF_BOUND;
	}

	if (fls.erase_page_before_write)
	{
		uint32_t p_addr = (addr / FLASH_MEMORY_PAGE_SIZE) * FLASH_MEMORY_PAGE_SIZE;
		NRF_LOG_INFO("%s:%d: Erasing page starting from address: %lu", __func__, __LINE__, p_addr);
		err_code = nrf_fstorage_erase(&fstorage, p_addr, 1U, NULL);
		if (err_code != NRF_SUCCESS)
		{
			NRF_LOG_INFO(
				"%s:%d: Failed to erase page starting at 0x%x due to %s", 
				__func__, 
				__LINE__, 
				p_addr, 
				NRF_LOG_ERROR_STRING_GET(err_code));
			return FLASH_MEMORY_ERR_FAILED_TO_ERASE;
		}
	}

	NRF_LOG_INFO("%s:%d: Start writing from %lu", __func__, __LINE__, addr);

	if (!fls.ignore_control_w)
	{
		NRF_LOG_INFO("%s:%d: Writing control word: %lu", __func__, __LINE__, *control_w);

		if (*control_w == FLASH_MEMORY_DEFAULT_VALUE)
		{
			return FLASH_MEMORY_ERR_INVALID_CONTROL_W;
		}

		bool successful = flash_memory_write_or_bufferize(addr, 1U, control_w);
		if (!successful)
		{
			NRF_LOG_ERROR("%s:%d: Failed to write control word", __func__, __LINE__);
			return FLASH_MEMORY_ERR_FAILED_TO_WRITE;
		}

		addr += sizeof(uint32_t);
	}

	bool successful = flash_memory_write_or_bufferize(addr, buf_size, buffer);
	if (!successful)
	{
		NRF_LOG_ERROR("%s:%d: Failed to write data", __func__, __LINE__);
		return FLASH_MEMORY_ERR_FAILED_TO_WRITE;
	}

	NRF_LOG_INFO("%s:%d: Successfully saved words", __func__, __LINE__);

	return FLASH_MEMORY_NO_ERR;
}

uint32_t flash_memory_seek_page_first_free_addr(uint32_t page_addr)
{
	if (page_addr % FLASH_MEMORY_PAGE_SIZE != 0)
	{
		return 0U;
	}

	uint32_t data = 0U;

	uint32_t * data_addr = (uint32_t *)page_addr;
	uint32_t start_offset = 0U;
	uint32_t end_offset = FLASH_MEMORY_PAGE_SIZE / sizeof(uint32_t) - 1;
	ret_code_t err_code = NRF_SUCCESS;
	
	err_code = nrf_fstorage_read(&fstorage, (uint32_t)(data_addr + end_offset), &data, sizeof(data));
	if (err_code != NRF_SUCCESS)
	{
		NRF_LOG_ERROR("%s:%d: Failed to read data from the flash at 0x%x.", __func__, __LINE__, page_addr + end_offset);
		return 0U;
	}
	if (data != FLASH_MEMORY_DEFAULT_VALUE)
	{
		return 0U;
	}

	while (start_offset < end_offset)
	{
		uint32_t mid_offset = end_offset - (end_offset - start_offset) / 2;

		err_code = nrf_fstorage_read(&fstorage, (uint32_t)(data_addr + mid_offset), &data, sizeof(data));
		if (err_code != NRF_SUCCESS)
		{
			NRF_LOG_ERROR("%s:%d: Failed to read data from the flash at 0x%x.", __func__, __LINE__, page_addr + mid_offset);
			return 0U;
		}
		
		if (data != FLASH_MEMORY_DEFAULT_VALUE)
		{
			start_offset = mid_offset + 1;
		}
		else
		{
			end_offset = mid_offset - 1;
		}
	}

	err_code = nrf_fstorage_read(&fstorage, (uint32_t)(data_addr + start_offset), &data, sizeof(data));
	if (err_code != NRF_SUCCESS)
	{
		NRF_LOG_ERROR("%s:%d: Failed to read data from the flash at 0x%x.", __func__, __LINE__, page_addr + start_offset);
		return 0U;
	}
	if (data != FLASH_MEMORY_DEFAULT_VALUE)
	{
		return (uint32_t)(data_addr + start_offset + 1);
	}

	err_code = nrf_fstorage_read(&fstorage, (uint32_t)(data_addr + end_offset), &data, sizeof(data));
	if (err_code != NRF_SUCCESS)
	{
		NRF_LOG_ERROR("%s:%d: Failed to read data from the flash at 0x%x.", __func__, __LINE__, page_addr + end_offset);
		return 0U;
	}
	if (data != FLASH_MEMORY_DEFAULT_VALUE)
	{
		return (uint32_t)(data_addr + end_offset + 1);
	}
	
	return 0U;
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

	struct flash_memory_flags_s fls = flash_memory_parse_flags(flags);

	ret_code_t err_code = NRF_SUCCESS;

	uint32_t s_addr = flash_memory_seek_page_first_free_addr(page_addr);
	if (s_addr == 0U || (s_addr + buf_size * sizeof(uint32_t)) > (page_addr + FLASH_MEMORY_PAGE_SIZE))
	{
		if (!fls.erase_if_necessary)
		{
			return FLASH_MEMORY_ERR_NOT_ENOUGH_SPACE;
		}

		err_code = nrf_fstorage_erase(&fstorage, page_addr, 1U, NULL);
		if (err_code != NRF_SUCCESS)
		{
			NRF_LOG_INFO(
				"%s:%d: Failed to erase page starting at 0x%x due to %s", 
				__func__, 
				__LINE__, 
				page_addr,
				NRF_LOG_ERROR_STRING_GET(err_code));
			return FLASH_MEMORY_ERR_FAILED_TO_ERASE;
		}

		s_addr = page_addr;
	}
	
	return flash_memory_write(s_addr, buffer, buf_size, control_w, flags);
}
