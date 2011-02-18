/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include <time.h>
#include <pthread.h>

#include "fstoraged_events.h"
#include "fstoraged_engine.h"

#include "fstoraged_http.h"

int
fsd_engine_ctx_init(fsd_engine_ctx_t **ctx)
{
	
	if ((*ctx = malloc(sizeof(fsd_engine_ctx_t))) == NULL)
		return (ERR_MEMORY);
	
        return (ERR_OK);
}

void
fsd_engine_ctx_fini(fsd_engine_ctx_t *ctx)
{
	
	if (ctx == NULL)
		return;
	
	free(ctx);
}

int
fsd_engine_ctx_create(fsd_conf_t *cfg, fs_log_t *log, fsd_engine_ctx_t **ctx)
{
	int err;
	
	if ((err = fsd_engine_ctx_init(ctx)) != ERR_OK)
		return (err);
	
	(*ctx)->cfg = cfg;
	(*ctx)->log = log;
	
	return (ERR_OK);
}

int
fsd_engine_run(fsd_conf_t *cfg, fs_log_t *log)
{
	int err, rc;
	pthread_t tid;
	fsd_engine_ctx_t *ctx;
	
	if ((err = fsd_engine_ctx_create(cfg, log, &ctx)) == ERR_OK) {
		rc = pthread_create(&tid, NULL, fsd_engine_thread, (void *)ctx);
		if (rc)
			fs_error(log, "pthread_create errcode = %d", rc);
	} else
		fs_error(log, "Cannot initialize engine context.");
	
	return (err);
}

void *
fsd_engine_thread(void *data)
{
	int err;
	unsigned int i;
	fsd_engine_ctx_t *ctx = (fsd_engine_ctx_t *)data;
	
	fsd_evt_t *evt;
	char *ptr;
	fsd_http_hdr_t *http_hdr;
	
	pthread_detach(pthread_self());
	
	fsd_equeue_init();
	
	if ((err = fsd_http_hdr_init(&http_hdr)) != ERR_OK)
		fs_error(ctx->log, "Could not create HTTP HDR: %s", fs_strerror(err));
	
	if ((err = fsd_http_hdr_set_attr(http_hdr, "Content-length", "256")) != ERR_OK) 
		fs_error(ctx->log, "Could not set HTTP HDR ATTR: %s", fs_strerror(err));
	
	fs_debug(ctx->log, "DECODED URL = %s", fsd_http_url_decode("http%3A%2F%2F"));
	
	i = 1;
	while (1) {
		sleep(1);
		//fs_debug(ctx->log, "engine iteration = %d", i);
		//fs_debug(ctx->log, "HTTP HDR ATTR Content-length=%s", fsd_http_hdr_get_attr(http_hdr, "Content-length"));
		if ((err = fsd_evt_create(i, (unsigned char *)"dupa", strlen("dupa") + 1, &evt)) != ERR_OK) {
			fs_error(ctx->log, "Could not create EVT: %s", fs_strerror(err));
			continue;
		}
		
		if ((err = fsd_equeue_insert(evt)) != ERR_OK) {
			fs_error(ctx->log, "Could not insert EVT to QUEUE (err=%d): %s", err, fs_strerror(err));
			continue;
		}
		
		if ((i % 2) == 0) {
			
			evt = fsd_equeue_pop();
			//fs_debug(ctx->log, "Pop EVT, type = %d, size = %d", evt->type, evt->size);
			fsd_evt_fini(evt);
			
			evt = fsd_equeue_pop();
			ptr = (char *)evt;
			//fs_debug(ctx->log, "Pop EVT, type = %d, size = %d, value = %s", evt->type, evt->size, (const char *)(ptr+sizeof(fsd_evt_t)) );
			fsd_evt_fini(evt);
			
		}
		
		i++;
	}
	
	return (NULL);
}
