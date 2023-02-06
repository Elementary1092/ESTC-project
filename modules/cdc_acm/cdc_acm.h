#ifndef ESTC_CDC_ACM_H
#define ESTC_CDC_ACM_H

#include <sys/types.h>
#include <stdlib.h>
#include <app_usbd_cdc_acm.h>

#include "cdc_acm_read_buf.h"

/**
 * @brief Events which may be encountered by this module.
*/
typedef enum
{
	CDC_ACM_SUCCESS,         /**< Operation finished successfully. */
	CDC_ACM_ACTION_ERROR,    /**< Got some error. */
	CDC_ACM_READ_WHITESPACE, /**< Read some whitespace character. */
	CDC_ACM_READ_NEW_LINE    /**< Read new line character. */
} cdc_acm_ret_code_t;

/**
 * @brief Initializes app_usbd_cdc_acm and registers it.
 * 
 * @param[in] acm Usbd cdc acm instance which should be registered.
*/
void cdc_acm_init(app_usbd_cdc_acm_t const *acm);

/**
 * @brief Reads entered char from cdc acm instance to the nrfx cdc acm read buffer 
 * 
 * @details If read buf has enough space read is successfully completed 
 *          and read character is written back to cdc acm instance.
 * 
 * @attention If the read buffer is full, read character cannot be processed later.
 *            This means that this function does not care 
 *            if a read character is successfully loaded to the read buffer or not.
 * 
 * @param[in] acm Registered cdc acm instance.
 * 
 * @param[in] read_buf Buffer where read character will be stored.
 * 
 * @retval @ref CDC_ACM_SUCCESS Function read a character successfully.
 * 
 * @retval @ref CDC_ACM_ACTION_ERROR app_usbd_cdc_acm_read() returns error.
 * 
 * @retval @ref CDC_ACM_READ_WHITESPACE Function read ' ' or '\t'.
 * 
 * @retval @ref CDC_ACM_READ_NEW_LINE: when function read '\r' or '\n'.
*/
cdc_acm_ret_code_t cdc_acm_echo(app_usbd_cdc_acm_t const *acm, cdc_acm_read_buf_t *read_buf);

/**
 * @brief Reads a character from the nrfx cdc acm instance.
 * 
 * @attention Should be used only while handling PORT_OPEN event or to read unnecessary character.
 * 
 * @param[in] acm Registered cdc acm instance from where a character is expected.
*/
void cdc_acm_only_read(app_usbd_cdc_acm_t const *acm);

/**
 * @brief Write a string to the registered cdc acm instance.
 * 
 * @param[in] acm Registered cdc acm instance.
 * 
 * @param[in] str String that should be written to the cdc acm instance.
 * 
 * @param[in] str_len A number of characters which should be written to the cdc acm instance.
 * 
 * @retval @ref CDC_ACM_SUCCESS Successfully written to the cdc acm instance.
 * 
 * @retval @ref CDC_ACM_ACTION_ERROR app_usbd_cdc_acm_write() has returned an error.
*/
cdc_acm_ret_code_t cdc_acm_write(app_usbd_cdc_acm_t const *acm, const char *str, ssize_t str_len);

#endif