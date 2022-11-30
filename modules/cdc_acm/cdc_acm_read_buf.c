#include <string.h>
#include "cdc_acm_read_buf.h"

void cdc_acm_read_buf_ctx_init(cdc_acm_read_buf_ctx_t *buf_ctx, char *buf, size_t buf_size)
{
	buf_ctx->buf = buf;
	buf_ctx->buf_size = buf_size;
	buf_ctx->curr_idx = 0;
}

void cdc_acm_read_buf_ctx_clear(cdc_acm_read_buf_ctx_t *buf_ctx)
{
	memset(buf_ctx->buf, 0, buf_ctx->buf_size);
	buf_ctx->curr_idx = 0;
}
