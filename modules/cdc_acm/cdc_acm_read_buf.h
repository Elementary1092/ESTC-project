#ifndef CDC_ACM_READ_BUF_H
#define CDC_ACM_READ_BUF_H

#include <stdlib.h>

typedef struct
{
	char   *buf;
	size_t buf_size;
	size_t curr_idx;
} cdc_acm_read_buf_ctx_t;

void cdc_acm_read_buf_ctx_init(cdc_acm_read_buf_ctx_t *buf_ctx, char *buf, size_t buf_size);

/*
	cdc_acm_read_buf_ctx_clear sets all values of buf to 0 and resets curr_idx.
*/
void cdc_acm_read_buf_ctx_clear(cdc_acm_read_buf_ctx_t *buf_ctx);

#endif