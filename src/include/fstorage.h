/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_H_
#define _FSTORAGE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#ifndef	_FSTORAGE_LOG_H_
typedef struct s_fs_log {
	int magic;
} fs_log_t;
#endif

#ifndef	_FSTORAGE_BACKEND_H_
typedef struct s_fs_backend {
	int magic;
} fs_backend_t;
#endif

#ifndef	_FSTORAGE_LOG_H_
typedef struct s_fs_conf {
	int magic;
} fs_conf_t;
#endif

#ifndef	_FSTORAGE_CONTEXT_H_
typedef struct s_fs_ctx {
	int magic;
} fs_ctx_t;
#endif

/** Fstorage ANSI C interface */

int fstorage_init(const char *fsuri, unsigned int debug, const char *logfile, fs_ctx_t **ctxp);
void fstorage_fini(fs_ctx_t *ctx);

const char *fstorage_strerror(int errcode);

int fstorage_file_send(fs_ctx_t *ctx, const char *fkey, const char *filepath);
int fstorage_file_recv(fs_ctx_t *ctx, const char *fkey, const char *filepath, size_t *sizep);
int fstorage_file_replace(fs_ctx_t *ctx, const char *fkey, const char *filepath);
int fstorage_file_remove(fs_ctx_t *ctx, const char *fkey);

int fstorage_hash(const char *key, char *hashp);

int fstorage_ns_encode(uint64_t num, uint8_t base, char *ciphertextp);
int fstorage_ns_decode(const char *ciphertext, uint8_t base, uint64_t *num);

int fstorage_akey_create(uint32_t expire_time, const char *fkey, uint64_t salt, uint8_t base, char *akeyp);
bool fstorage_akey_check(const char *akey, const char *fkey, uint64_t salt, uint8_t base);

int fstorage_uri(const char *filename, const char *fkey, const char *akey, char *urip);
int fstorage_http_url(fs_ctx_t *ctx, const char *filename, const char *fkey, const char *akey, char *urlp);
int fstorage_https_url(fs_ctx_t *ctx, const char *filename, const char *fkey, const char *akey, char *urlp);

#endif  /* !_FSTORAGE_H_ */

