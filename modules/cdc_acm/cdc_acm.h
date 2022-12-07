#ifndef ESTC_CDC_ACM_H
#define ESTC_CDC_ACM_H

#include <sys/types.h>
#include <stdlib.h>
#include <app_usbd_cdc_acm.h>

#include "cdc_acm_read_buf.h"

typedef enum
{
	CDC_ACM_SUCCESS,
	CDC_ACM_ACTION_ERROR,
	CDC_ACM_READ_WHITESPACE,
	CDC_ACM_READ_NEW_LINE
} cdc_acm_ret_code_t;

void cdc_acm_init(app_usbd_cdc_acm_t const *acm);

/*
	cdc_acm_echo reads entered char from { acm } to { read_buf } if read_buf has enough space
		and redirects this char to { acm }.
	Returns:
		- CDC_ACM_SUCCESS: when function read successfully;
		- CDC_ACM_ACTION_ERROR: when app_usbd_cdc_acm_read() returns error;
		- CDC_ACM_READ_WHITESPACE: when function read ' ' or '\t';
		- CDC_ACM_READ_NEW_LINE: when function read '\r' or '\n'.
*/
cdc_acm_ret_code_t cdc_acm_echo(app_usbd_cdc_acm_t const *acm, cdc_acm_read_buf_ctx_t *read_buf);

/*
	cdc_acm_only_read reads a character from the { acm }.
	Should be used only while handling PORT_OPEN event or to read unnecessary character.
*/
void cdc_acm_only_read(app_usbd_cdc_acm_t const *acm);

/*
	cdc_acm_write write { buf_size } chars from { str } to { acm }.
	Returns:
		- CDC_ACM_SUCCESS: when function wrote successfully;
		- CDC_ACM_ACTION_ERROR: when app_usbd_cdc_acm_write() returns error;
*/
cdc_acm_ret_code_t cdc_acm_write(app_usbd_cdc_acm_t const *acm, const char *str, ssize_t str_len);

#endif