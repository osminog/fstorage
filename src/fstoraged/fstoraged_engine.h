/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGED_ENGINE_H_
#define	_FSTORAGED_ENGINE_H_

#include "fstoraged_conf.h"
#include "fstoraged_session.h"
#include "fstoraged_events.h"

typedef struct s_fsd_engine_ctx {
	fsd_conf_t *cfg;
	fs_log_t *log;
} fsd_engine_ctx_t;

int fsd_engine_ctx_init(fsd_engine_ctx_t **ctx);
void fsd_engine_ctx_fini(fsd_engine_ctx_t *ctx);
int fsd_engine_ctx_create(fsd_conf_t *cfg, fs_log_t *log, fsd_engine_ctx_t **ctx);

int fsd_engine_run(fsd_conf_t *cfg, fs_log_t *log);
void *fsd_engine_thread(void *data);

#endif	/* !_FSTORAGED_ENGINE_H_ */
