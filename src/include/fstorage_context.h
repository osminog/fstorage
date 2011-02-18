/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGE_CONTEXT_H_
#define	_FSTORAGE_CONTEXT_H_

#include "fstorage_impl.h"
#include "fstorage_conf.h"
#include "fstorage_log.h"

int fstorage_ctx_init(fs_conf_t *cfg, fs_log_t *log, fs_ctx_t **ctxp);
void fstorage_ctx_fini(fs_ctx_t *ctx);

#endif	/* !_FSTORAGE_CONTEXT_H_ */
