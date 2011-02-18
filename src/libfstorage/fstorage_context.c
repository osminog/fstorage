/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_impl.h"
#include "fstorage_context.h"

int
fstorage_ctx_init(fs_conf_t *cfg, fs_log_t *log, fs_ctx_t **ctxp)
{
	
	if ((*ctxp = malloc(sizeof(*ctxp))) == NULL)
		return (ERR_MEMORY);
	
	(*ctxp)->cfg = cfg;
	(*ctxp)->log = log;
	
	return (ERR_OK);
}

void
fstorage_ctx_fini(fs_ctx_t *ctx)
{
	
	if (ctx == NULL)
		return;
	
	fstorage_conf_fini(ctx->cfg);
	fs_log_fini(ctx->log);
	
	free(ctx);
}
