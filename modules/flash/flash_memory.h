#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include <stdint.h>
#include "app_config.h"

#ifndef NRF_DFU_APP_DATA_AREA_SIZE
#error NRF_DFU_APP_DATA_AREA_SIZE is not defined
#endif

#if NRF_DFU_APP_DATA_AREA_SIZE < 8192
#error NRF_DFU_APP_DATA_AREA_SIZE is too small
#endif

#ifndef FLASH_MEMORY_DATA_LAST_ADDRESS
#define FLASH_MEMORY_DATA_LAST_ADDRESS     0xE0000
#endif

#define FLASH_MEMORY_DATA_STARTING_ADDRESS 0xE0000 - NRF_DFU_APP_DATA_AREA_SIZE
#define FLASH_MEMORY_DATA_END_ADDRESS      FLASH_MEMORY_DATA_STARTING_ADDRESS + NRF_DFU_APP_DATA_AREA_SIZE
#define FLASH_MEMORY_PAGE_SIZE             4096
#define FLASH_MEMORY_FIRST_PAGE            FLASH_MEMORY_DATA_STARTING_ADDRESS
#define FLASH_MEMORY_SECOND_PAGE           FLASH_MEMORY_FIRST_PAGE + FLASH_MEMORY_PAGE_SIZE

typedef enum
{
	FLASH_MEMORY_NO_FLAGS                = 1,
	FLASH_MEMORY_IGNORE_CONTROL_W        = 2,
	FLASH_MEMORY_IGNORE_DEFAULT_VALUES   = 4,
	FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE = 8,
	FLASH_MEMORY_ERASE_PAGE_IF_NECESSARY = 16
} flash_memory_flag_t;

typedef enum
{
	FLASH_MEMORY_NO_ERR                    = 0,
	FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE  = 1,
	FLASH_MEMORY_ERR_INVALID_CONTROL_W     = 2,
	FLASH_MEMORY_ERR_POSSIBLY_INVALID_DATA = 3,
	FLASH_MEMORY_ERR_INVALID_PAGE_ADDR     = 4,
	FLASH_MEMORY_ERR_NOT_ENOUGH_SPACE      = 5,
	FLASH_MEMORY_ERR_ADDR_OUT_OF_BOUND     = 6
} flash_memory_err_t;

/*
	flash_memory_read tries to read { limit } words starting from APP_DATA_AREA + { offset } and place them to the { buffer }.

	If FLASH_MEMORY_NO_FLAGS flag is passed all other flags will be ignored.

	If FLASH_MEMORY_IGNORE_CONTROL_W flag is NOT passed, function expects the 1st word to be equal to { control_w }.
		If { control_w } == 0xFFFFFFFF, FLASH_MEMORY_ERR_INVALID_CONTROL_W error will be returned.
		If { control_w } == 1st word, 1st word will be ignored and not be placed to the { buffer }.
		Otherwise, will return FLASH_MEMORY_ERR_INVALID_CONTROL_W error.

	If FLASH_MEMORY_IGNORE_DEFAULT_VALUES flag is NOT passed, function expects all read values to be != 0xFFFFFFFF.
		If any word in the range ( ( APP_DATA_AREA + { offset } ) ... ( APP_DATA_AREA + { offset } - limit ) ) == 0xFFFFFFF,
			FLASH_MEMORY_ERR_POSSIBLE_INVALID_DATA error will be returned.

	Other flags are ignored by this function.
*/
flash_memory_err_t flash_memory_read(uint32_t addr,
									uint32_t *buffer,
									uint32_t limit,
									uint32_t control_w, 
									flash_memory_flag_t flags);

/*
	flash_memory_write tries to write all words from the { buffer } to the flash memory starting from APP_DATA_AREA + { offset }.

	If FLASH_MEMORY_NO_FLAGS flag is passed all other flags will be ignored.

	If FLASH_MEMORY_IGNORE_CONTROL_W flag is NOT passed, function writes { control_w } first (unless { control_w } == 0xFFFFFFFF).
		If { control_w } == 0xFFFFFFFF, FLASH_MEMORY_ERR_INVALID_CONTROL_W error will be returned.

	If FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE flag is passed, page will be erased first
		and FLASH_MEMORY_ERASE_PAGE_IF_NEEDED flag will be ignored.

	Other flags are ignored by this function.

	Returns FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE error if this function could not write to the flash memory.
*/
flash_memory_err_t flash_memory_write(uint32_t addr,
									uint32_t *buffer, 
									uint32_t buf_size,
									uint32_t control_w, 
									flash_memory_flag_t flags);

/*
	flash_memor_seek_page_first_free_addr searches for first slot in the page where data != DEFAULT_VALUE.
	Requires all data to be written to the page sequentially.
	Returns:
		- 0 - if page is page number is specified incorrectly, or page is full, or such slot was not found.
		- address - if such slot was found.
*/
uint32_t flash_memory_seek_page_first_free_addr(uint32_t page_addr);

/*
	flash_memory_page_append - tries to append record to the page.
	Returns:
		- FLASH_MEMORY_ERR_NOT_ENOUGH_SPACE - if FLASH_MEMORY_ERASE_PAGE_IF_NECCESSARY is not specified and page is full.
		- FLASH_MEMORY_ERR_ADDRESS_OUT_OF_BOUND - if address specified is not in the DATA_AREA
		- FLASH_MEMORY_INVALID_CONTROL_W - if { control_w } == DEFAULT_VALUE
		- FLASH_MEMORY_WORD_IS_NOT_WRITABLE - if data cannot be written.
		- FLASH_MEMORY_NO_ERR - on success.
*/
flash_memory_err_t flash_memory_page_append(uint32_t *buffer, 
											uint32_t buf_size, 
											uint32_t page_addr, 
											uint32_t control_w,
											flash_memory_flag_t flags);

#endif