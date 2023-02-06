#ifndef CDC_ACM_READ_BUF_H
#define CDC_ACM_READ_BUF_H

#include <stdlib.h>

typedef struct
{
	char   *buf;
	size_t buf_size;
	size_t curr_idx;
} cdc_acm_read_buf_t;

void cdc_acm_read_buf_init(cdc_acm_read_buf_t *buf_ctx, char *buf, size_t buf_size);

/*
	cdc_acm_read_buf_ctx_clear sets all values of buf to 0 and resets curr_idx.
*/
void cdc_acm_read_buf_clear(cdc_acm_read_buf_t *buf_ctx);

void cdc_acm_read_buf_append(cdc_acm_read_buf_t *buf_ctx, const char c);

#endif