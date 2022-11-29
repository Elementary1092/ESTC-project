#ifndef ESTC_CDC_ACM_H
#define ESTC_CDC_ACM_H

#include <stdlib.h>
#include <app_usbd_cdc_acm.h>

void cdc_acm_init(app_usbd_cdc_acm_t const *acm);

/*
	cdc_acm_echo reads entered { buf_size } chars from { acm } to { buf }
		and redirects all read chars to { acm }.
	If this function encounters whitespace it stops reading and returns.
	
	Function returns number of character read.
*/
ssize_t cdc_acm_echo(app_usbd_cdc_acm_t const *acm, char *buf, ssize_t buf_size);

/*
	cdc_acm_read reads entered { buf_size } chars from { acm } to { buf }.
	If this function encounters whitespace it stops reading and returns.

	Function returns number of character read.
*/
ssize_t cdc_acm_read(app_usbd_cdc_acm_t const *acm, char *buf, ssize_t buf_size);

/*
	cdc_acm_write write { buf_size } chars from { str } to { acm }.
*/
void cdc_acm_write(app_usbd_cdc_acm_t const *acm, const char *str, ssize_t str_len);

#endif