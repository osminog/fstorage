/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_IMPL_H_
#define _FSTORAGE_IMPL_H_

#include <sys/types.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "fstorage_ns.h"
#include "fstorage_str.h"
#include "fstorage_btp.h"
#include "fstorage_keys.h"
#include "fstorage_cfg.h"
#include "fstorage_common.h"
#include "fstorage_opcode.h"
#include "fstorage_errcode.h"

#define FS_DEFAULT_LOGLEVEL	FS_LOG_DEBUG
#define FS_DEFAULT_TIMEOUT	60

#define	FS_DEFAULT_SALT		985609251093	/* long long integer */
#define FS_DEFAULT_BASE		45		/* 2, 3, 4, ... , 62 */
#define FS_DEFAULT_EXPIRE_TIME	3600 		/* seconds */

#define	FS_BASE_MIN		2
#define	FS_BASE_MAX		62

#define FS_PROTO_NAME		"fstp"

#define	FS_STATE_DISCONNECTED	0
#define	FS_STATE_CONNECTED	1
#define FS_STATE_AUTHORIZED	2
#define	FS_STATE_HANDSHAKED	3

#define	FS_IN			1
#define	FS_OUT			2

#define	FS_BACKENDS_LIMIT	512

#define	FS_LOG_ERROR		1
#define	FS_LOG_WARN		2
#define	FS_LOG_INFO		3
#define	FS_LOG_DEBUG		4

typedef struct s_fs_log {
	int level;
	FILE *fd;
} fs_log_t;

typedef struct s_fs_backend {
	uint16_t port;
	char host[128];
	char name[128];
	char proto[16];
} fs_backend_t;

typedef struct s_fs_conf {
	int debug;
	char logfile[256];
	fs_backend_t *backends[FS_BACKENDS_LIMIT];
	unsigned int backends_cnt;
} fs_conf_t;

typedef struct s_fs_ctx {
	fs_conf_t *cfg;
	fs_log_t *log;
} fs_ctx_t;

#include "fstorage_log.h"
#include "fstorage_backend.h"
#include "fstorage_conf.h"
#include "fstorage_context.h"

#endif  /* !_FSTORAGE_IMPL_H_ */
