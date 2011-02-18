/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_LOG_H_
#define	_FSTORAGE_LOG_H_

#include <stdio.h>
#include <stdarg.h>

#include "fstorage_impl.h"

int fs_log_init(const char *filepath, fs_log_t **logp);
void fs_log_fini(fs_log_t *log);

void fs_log_set_level(fs_log_t *log, int level);
int fs_log_get_level(fs_log_t *log);

void fs_log_arg(fs_log_t *log, btp_arg_t *arg);
void fs_log_pkg(fs_log_t *log, btp_pkg_t *pkg, int direct);

void _fs_log(int level, fs_log_t *log, const char *format, ...);

#define	fs_error(log, ...)		_fs_log(FS_LOG_ERROR, log, "[ ERROR ] " __VA_ARGS__)
#define	fs_warn(log, ...)		_fs_log(FS_LOG_WARN,  log, "[  WARN ] " __VA_ARGS__)
#define	fs_info(log, ...)		_fs_log(FS_LOG_INFO,  log, "[  INFO ] " __VA_ARGS__)
#define	fs_debug(log, ...)		_fs_log(FS_LOG_DEBUG, log, "[ DEBUG ] " __VA_ARGS__)

#endif /* !_FSTORAGE_LOG_H_ */
