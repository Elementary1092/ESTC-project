#ifndef ESTC_CDC_ACM_H
#define ESTC_CDC_ACM_H

#include <stdlib.h>
#include <app_usbd_cdc_acm.h>

void cdc_acm_init(app_usbd_cdc_acm_t acm);

/*
	cdc_acm_echo reads entered { buf_size } chars from { acm } to { buf }
		and redirects all read chars to { acm }.
	If this function encounters whitespace it stops reading and returns.
*/
void cdc_acm_echo(app_usbd_cdc_acm_t acm, char *buf, size_t buf_size);

/*
	cdc_acm_read reads entered { buf_size } chars from { acm } to { buf }.
	If this function encounters whitespace it stops reading and returns.
*/
void cdc_acm_read(app_usbd_cdc_acm_t acm, char *buf, size_t buf_size);

/*
	cdc_acm_write write { buf_size } chars from { str } to { acm }.
*/
void cdc_acm_write(app_usbd_cdc_acm_t acm, const char *str, size_t str_len);

#endif