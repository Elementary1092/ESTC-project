#ifndef ESTC_HSV_PICKER_FLASH_MEMORY_H
#define ESTC_HSV_PICKER_FLASH_MEMORY_H

#include "hsv_converter.h"

/**
 * @brief Tries to load from the flash the last saved state of the hsv picker.
 * 
 * @retval !(.is_valid) If failed to load the last state.
 * 
 * @retval .is_valid If the last state is loaded successfully.
*/
hsv_ctx_t hsv_picker_flash_try_init(void);

/**
 * @brief Tries to save passed state of the hsv picker.
 * 
 * @param[in] hsv The state of hsv picker.
*/
bool hsv_picker_flash_update_saved_value(hsv_ctx_t const *hsv);

#endif