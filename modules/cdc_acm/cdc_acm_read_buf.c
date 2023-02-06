#include <string.h>
#include "cdc_acm_read_buf.h"

void cdc_acm_read_buf_init(cdc_acm_read_buf_t *buf_ctx, char *buf, size_t buf_size)
{
	buf_ctx->buf = buf;
	buf_ctx->buf_size = buf_size;
	buf_ctx->curr_idx = 0;
	memset(buf, 0, buf_size);
}

void cdc_acm_read_buf_clear(cdc_acm_read_buf_t *buf_ctx)
{
	memset(buf_ctx->buf, 0, buf_ctx->buf_size);
	buf_ctx->curr_idx = 0;
}

void cdc_acm_read_buf_append(cdc_acm_read_buf_t *buffer, const char c)
{
	if (buffer->curr_idx < (buffer->buf_size - 1))
	{
		buffer->buf[buffer->curr_idx++] = c;
	}
}
