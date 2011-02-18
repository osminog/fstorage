/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_log.h"
#include "fstorage_impl.h"

#ifndef fileno
int fileno(FILE *stream);
#endif


int
fs_log_init(const char *filepath, fs_log_t **logp)
{
	FILE *fd;
	
	if (filepath != NULL && filepath[0] != '\0') {
		if ((fd = fopen(filepath, "wb+")) != NULL) {
			dup2(fileno(fd), 2);
			fclose(fd);
		} else
			return (ERR_FILESYSTEM);
	}
	
	if ((*logp = malloc(sizeof(fs_log_t))) == NULL)
		return (ERR_MEMORY);
	
	(*logp)->level = FS_DEFAULT_LOGLEVEL;
	(*logp)->fd = stderr;
	
	return (ERR_OK);
}

void
fs_log_fini(fs_log_t *log)
{
	
	if (log == NULL)
		return;
	
	free(log);
}

void
fs_log_set_level(fs_log_t *log, int level)
{
	
	log->level = level;
}

int
fs_log_get_level(fs_log_t *log)
{
	
	return (log->level);
}

void
fs_log_arg(fs_log_t *log, btp_arg_t *arg)
{
	uint32_t size;
	int64_t number;
	char *str;
	unsigned char *binary;
	bool boolean;
	
	if (!btp_arg_check(arg))
		return;
	
	switch (btp_arg_type(arg)) {
		case BTP_TYPE_NUMBER:
			btp_arg_get_number(arg, &number),
			fs_debug(log, "| <num(%u)> | %s = %lu", sizeof(number), btp_arg_name(arg), number);
			break;
			
		case BTP_TYPE_STRING:
			btp_arg_get_string(arg, &str),
			fs_debug(log, "| <str(%u)> | %s = %s", strlen(str), btp_arg_name(arg), str);
			break;
			
		case BTP_TYPE_BINARY:
			btp_arg_get_binary(arg, &size, &binary),
			fs_debug(log, "| <bin(%u)> | %s = <binary>", size, btp_arg_name(arg));
			break;
			
		case BTP_TYPE_BOOL:
			btp_arg_get_bool(arg, &boolean),
			fs_debug(log, "| <num(%u)> | %s = %s", sizeof(bool), btp_arg_name(arg), boolean ? "true" : "false");
			break;
		
		default:
			fs_debug(log, "| <xxx(%u)> | %s = <unknoe data type>", btp_arg_vsize(arg) - BTP_VALUE_HEAD_SIZE, btp_arg_name(arg));
			break;
	}
}

void
fs_log_pkg(fs_log_t *log, btp_pkg_t *pkg, int direct)
{
	unsigned int cnt;
	btp_arg_t *arg;
	
	if (!btp_pkg_check(pkg))
		return;
	
	fs_debug(log, " ");
	
	if (direct == FS_IN)
		fs_debug(log, "<# ---------------- INCOMING PACKAGE -------------- # ");
	else if (direct == FS_OUT)
		fs_debug(log, "<# ---------------- OUTGOING PACKAGE -------------- # ");
	else
		fs_debug(log, "<# ---------------- ---------------- -------------- #");
	
	fs_debug(log, "| opcode = %d | error = %d | argno = %d | size = %lu",
		btp_pkg_get_opcode(pkg),
		btp_pkg_get_error(pkg),
		btp_pkg_argno(pkg),
		btp_pkg_size(pkg)
		);
	
	if (btp_pkg_argno(pkg) > 0) {
		fs_debug(log, "  ------------------------------------------------  ");
		BTP_FOREACH(cnt, arg, pkg)
			fs_log_arg(log, arg);
	}
	
	fs_debug(log, "# ------------------------------------------------ #> ");
	fs_debug(log, " ");
}

void
_fs_log(int level, fs_log_t *log, const char *format , ...)
{
	va_list args;
	
	if (level > log->level)
		return;
	
	va_start(args, format);
	vfprintf(log->fd, format, args);
	va_end(args);
	fprintf(log->fd, "\n");	
}
