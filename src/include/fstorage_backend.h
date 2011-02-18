/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGE_BACKEND_H_
#define	_FSTORAGE_BACKEND_H_

#include <stdint.h>
#include "fstorage_impl.h"

int fstorage_backend_init(const char *proto, const char *name, const char *host, uint16_t port, fs_backend_t **backend);
void fstorage_backend_fini(fs_backend_t *backend);
void fstorage_backends_fini(fs_backend_t **backends);

int fstorage_backend_parse(const char *str, fs_backend_t **backendp);
int fstorage_backends_parse(const char *str, unsigned int *cnt, fs_backend_t ***backendsp);

int fstorage_get_backend_by_name(fs_ctx_t *ctx, const char *backend_name, unsigned int *id_backendp);
int fstorage_get_backend_shuffle(fs_ctx_t *ctx, unsigned int *id_backendp);

#endif	/* !_FSTORAGE_BACKEND_H_ */
