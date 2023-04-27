#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include <stdint.h>
#include "app_config.h"

#ifndef NRF_DFU_APP_DATA_AREA_SIZE
#error NRF_DFU_APP_DATA_AREA_SIZE is not defined
#endif

#if NRF_DFU_APP_DATA_AREA_SIZE < 20480
#error NRF_DFU_APP_DATA_AREA_SIZE is too small
#endif

#ifndef FLASH_MEMORY_DATA_LAST_ADDRESS
#define FLASH_MEMORY_DATA_LAST_ADDRESS     0xE0000
#endif

#define FLASH_MEMORY_DATA_STARTING_ADDRESS FLASH_MEMORY_DATA_END_ADDRESS - NRF_DFU_APP_DATA_AREA_SIZE
#define FLASH_MEMORY_DATA_END_ADDRESS      FLASH_MEMORY_DATA_STARTING_ADDRESS + 4096 * 2
#define FLASH_MEMORY_PAGE_SIZE             4096
#define FLASH_MEMORY_FIRST_PAGE            FLASH_MEMORY_DATA_STARTING_ADDRESS
#define FLASH_MEMORY_SECOND_PAGE           FLASH_MEMORY_FIRST_PAGE + FLASH_MEMORY_PAGE_SIZE

#define FLASH_MEMORY_DEFAULT_VALUE 0xFFFFFFFF

/**
 * @brief Flags to change behavour of functions in this module.
*/
typedef enum
{
	FLASH_MEMORY_NO_FLAGS                = 1, /**< Default behavour */
	FLASH_MEMORY_IGNORE_CONTROL_W        = 2, /**< Should not compare given control word and control word from the memory*/
	FLASH_MEMORY_IGNORE_DEFAULT_VALUES   = 4, /**< Ignore 0xFFFFFFFF */
	FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE = 8, /**< Page is cleared before writing to it. Recomended if overwritting is needed. */
	FLASH_MEMORY_ERASE_PAGE_IF_NECESSARY = 16 /**< If value is not writable, then clear the page.*/
} flash_memory_flag_t;

/**
 * @brief Possible return values of functions 
*/
typedef enum
{
	FLASH_MEMORY_NO_ERR,                    /**< Execution is successful. */
	FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE,  /**< Failed to write. */
	FLASH_MEMORY_ERR_INVALID_CONTROL_W,     /**< Failed to validate control word. */
	FLASH_MEMORY_ERR_POSSIBLY_INVALID_DATA, /**< Possibly got default values (0xFFFFFFFF). */
	FLASH_MEMORY_ERR_INVALID_PAGE_ADDR,     /**< Address of a page is invalid. */
	FLASH_MEMORY_ERR_NOT_ENOUGH_SPACE,      /**< Record cannot be written to the page. */
	FLASH_MEMORY_ERR_ADDR_OUT_OF_BOUND,     /**< Address is not accessible. */
	FLASH_MEMORY_ERR_FAILED_TO_WRITE,       /**< Failed to write due to softdevice issues */
	FLASH_MEMORY_ERR_FAILED_TO_ERASE,       /**< Failed to erase page due to softdevice issues */
	FLASH_MEMORY_ERR_FAILED_TO_READ,        /**< Failed to read data due to softdevice issues */
} flash_memory_err_t;

/**
 * @brief Initializes flash_memory_sd. Not used in flash_memory (nvmc).
*/
void flash_memory_init(void);

/**
 * @brief Read certain amount of uint32_t type integers from the memory.
 * 
 * @details Tries to read { limit } words starting from APP_DATA_AREA + { offset } and place them to the { buffer }.
 * 
 * @param[in]  addr Starting address of the record.
 * 
 * @param[out] buffer Buffer where all read values will be placed.
 * 
 * @param[in]  limit Number of uint32_t integers to be placed to the buffer.
 * 
 * @param[in]  control_w Some number which validates record (optional) (will not be placed to the buffer).
 * 
 * @param[in]  flags Possible flags: \n
 *                   - @ref FLASH_MEMORY_NO_FLAGS: ignore all other flags. \n 
 *                   - @ref FLASH_MEMORY_IGNORE_CONTROL_W: do not validate the record. \n 
 *                   - @ref FLASH_MEMORY_IGNORE_DEFAULT_VALUES: ignore 0xFFFFFFFF and read { limit } records. \n 
 *                   - other flags: are ignored. \n 
 *                   To pass multiple flags use bitwise OR.
 * 
 * @retval @ref FLASH_MEMORY_ERR_INVALID_CONTROL_W     If @ref FLASH_MEMORY_IGNORE_CONTROL_W is not passed
 *                                                     and control word from the memory != passed { control_w };
 *                                                     or control_w == 0xFFFFFFFF.
 * 
 * @retval @ref FLASH_MEMORY_ERR_POSSIBLE_INVALID_DATA If @ref FLASH_MEMORY_IGNORE_DEFAULT_VALUES is not paseed
 *                                                     and some record == 0xFFFFFFFF.
 * 
 * @retval @ref FLASH_MEMORY_NO_ERR                    Successful execution.
*/
flash_memory_err_t flash_memory_read(uint32_t addr,
									uint32_t *buffer,
									uint32_t limit,
									uint32_t const *control_w, 
									flash_memory_flag_t flags);

/**
 * @brief Tries to save data in the flash memory.
 * 
 * @details Tries to write all words from the { buffer } to the flash memory starting from APP_DATA_AREA + { offset }.
 * 
 * @param[in] addr Starting address of the record.
 * 
 * @param[in] buffer Data which should be saved.
 * 
 * @param[in] buf_size Size of the array holding data.
 * 
 * @param[in] control_w Control word which will be used in validating data.
 * 
 * @param[in]  flags Possible flags: \n 
 *                   - @ref FLASH_MEMORY_NO_FLAGS: ignore all other flags. \n 
 *                   - @ref FLASH_MEMORY_IGNORE_CONTROL_W: do not store validating number. \n 
 *                   - @ref FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE: clear page. \n 
 *                   - other flags: are ignored. \n 
 *                   To pass multiple flags use bitwise OR.
 * 
 * @retval @ref FLASH_MEMORY_ERR_WORD_IS_NOT_WRITABLE record cannot be precisely stored.
 * 
 * @retval @ref FLASH_MEMORY_INVALID_CONTROL_WORD @ref control_w == 0xFFFFFFFF.
 * 
 * @retval @ref FLASH_MEMORY_ERR_ADDR_OUT_OF_BOUND invalid starting address.
 * 
 * @retval @ref FLASH_MEMORY_NO_ERR Written successfully.
*/
flash_memory_err_t flash_memory_write(uint32_t addr,
									uint32_t *buffer, 
									uint32_t buf_size,
									uint32_t const *control_w, 
									flash_memory_flag_t flags);

/**
 * @brief earches for first slot in the page where data != DEFAULT_VALUE.
 * 
 * @attention Requires all data to be written sequentially on the page.
 * 
 * @param[in] addr Starting address of the page.
 * 
 * @retval 0 Page starting address is not correct or Could not find an empty slot.
 * 
 * @retval address Empty slot is found in the page. 
*/
uint32_t flash_memory_seek_page_first_free_addr(uint32_t page_addr);

/**
 * @brief Tries to write data to the first empty slot of the page.
 * 
 * @param[in] page_addr Starting address of the page.
 * 
 * @param[in] buffer Data which should be saved.
 * 
 * @param[in] buf_size Size of the array holding data.
 * 
 * @param[in] control_w Control word which will be used in validating data.
 * 
 * @param[in] flags Possible flags: \n 
 *                   - @ref FLASH_MEMORY_NO_FLAGS: ignore all other flags. \n 
 *                   - @ref FLASH_MEMORY_IGNORE_CONTROL_W: do not store validating number. \n 
 *                   - @ref FLASH_MEMORY_ERASE_PAGE_BEFORE_WRITE: clear page. \n 
 *                   - @ref FLASH_MEMORY_ERASE_PAGE_IF_NECCESSARY: clear page if there is no space in the page to hold record. \n
 *                   - other flags: are ignored. \n
 *                   To pass multiple flags use bitwise OR. \n
 * 
 * @retval @ref FLASH_MEMORY_ERR_NOT_ENOUGH_SPACE If FLASH_MEMORY_ERASE_PAGE_IF_NECCESSARY is not specified and page is full.
 * 
 * @retval @ref FLASH_MEMORY_ERR_ADDRESS_OUT_OF_BOUND If address specified is not accessable.
 * 
 * @retval @ref FLASH_MEMORY_INVALID_CONTROL_W If @ref control_w == DEFAULT_VALUE
 * 
 * @retval @ref FLASH_MEMORY_WORD_IS_NOT_WRITABLE If data cannot be written.
 * 
 * @retval @ref FLASH_MEMORY_NO_ERR Data is appended successfully.
*/
flash_memory_err_t flash_memory_page_append(uint32_t *buffer, 
											uint32_t buf_size, 
											uint32_t page_addr, 
											uint32_t const *control_w,
											flash_memory_flag_t flags);

#endif