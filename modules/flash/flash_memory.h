#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include <stdint.h>

typedef enum
{
	FLASH_MEMORY_NO_FLAGS                = 1,
	FLASH_MEMORY_IGNORE_CONTROL_W        = 2,
	FLASH_MEMORY_IGNORE_DEFAULT_VALUES   = 4,
	FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE = 8
} flash_memory_flag_t;

typedef enum
{
	FLASH_MEMORY_NO_ERR                    = 0,
	FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE  = 1,
	FLASH_MEMORY_ERR_INVALID_CONTROL_W     = 2,
	FLASH_MEMORY_ERR_POSSIBLY_INVALID_DATA = 3
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
flash_memory_err_t flash_memory_read(uint32_t *buffer, uint32_t limit, uint32_t offset, uint32_t control_w, flash_memory_flag_t flags);

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
flash_memory_err_t flash_memory_write(uint32_t *buffer, uint32_t buf_size, uint32_t offset, uint32_t control_w, flash_memory_flag_t flags);

#endif