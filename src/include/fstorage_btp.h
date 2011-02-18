/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_BTP_H_
#define _FSTORAGE_BTP_H_

#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "fstorage_errcode.h"


/* TYPE BTP */
typedef struct s_btp {
        int fd;
        uint64_t snd_cnt;
        uint64_t rcv_cnt;
} btp_t;

/* TYPES BTP_PKG */
typedef unsigned char btp_pkg_t;

typedef struct s_btp_pkg_head {
        uint16_t magic;
        uint32_t opcode;
        uint32_t errcode;
        uint32_t argno;
        uint64_t size;
        uint64_t flags;
        /* arguments */
} __attribute((packed)) btp_pkg_head_t;

/* TYPES BTP_ARG */
typedef unsigned char btp_arg_t;

typedef struct s_btp_arg_head {
        uint16_t magic;
        uint32_t id;
        uint32_t size;
        uint32_t vsize;
        uint16_t nsize;
        uint8_t type;
        uint8_t	name[];
        /* value */
} __attribute((packed)) btp_arg_head_t;

/* TYPES BTP_VALUE */

typedef unsigned char btp_value_t;

typedef struct s_btp_value {
	uint16_t magic;
	uint32_t size;
	uint8_t type;
	bool boolean;
	int64_t number;
	unsigned char *binary;
	char *string;
} __attribute((packed)) btp_value_head_t;

#define	BTP_FILE_PART_SIZE	4096

#define BTP_PKG_HEAD_SIZE	sizeof(btp_pkg_head_t)
#define BTP_ARG_HEAD_SIZE	sizeof(btp_arg_head_t)
#define BTP_VALUE_HEAD_SIZE	sizeof(btp_value_head_t)

#define BTP_PKG_MAGIC           0x1b3
#define BTP_ARG_MAGIC           0xc12
#define BTP_VALUE_MAGIC         0x90f

#define BTP_TYPE_NUMBER     	1
#define BTP_TYPE_STRING		2
#define BTP_TYPE_BINARY     	3
#define BTP_TYPE_BOOL       	4

#define BTP_VALUE_SIZE_MAX	(BTP_VALUE_HEAD_SIZE + 65536) 
#define BTP_VALUE_SIZE_MIN 	BTP_VALUE_HEAD_SIZE
#define BTP_ARG_NAME_SIZE_MAX	512
#define BTP_ARG_NAME_SIZE_MIN	1

#define BTP_ARG_SIZE_MAX 	(BTP_VALUE_SIZE_MAX + BTP_ARG_NAME_SIZE_MAX + BTP_ARG_HEAD_SIZE)
#define BTP_ARG_SIZE_MIN	BTP_ARG_HEAD_SIZE

#define	BTP_PKG_ARGNO_MAX	4096
#define BTP_PKG_SIZE_MAX 	((BTP_ARG_SIZE_MAX * BTP_PKG_ARGNO_MAX) + BTP_PKG_HEAD_SIZE)
#define BTP_PKG_SIZE_MIN	BTP_PKG_HEAD_SIZE

#define BTP_CHECK_PKG(pkg) \
	(((pkg) != NULL)				&&  \
	((pkg)->magic == BTP_PKG_MAGIC)			&&  \
	(((pkg)->opcode > 0) || ((pkg)->errcode > 0))	&&  \
	((pkg)->argno <= BTP_PKG_ARGNO_MAX)		&&  \
	((pkg)->size >= BTP_PKG_SIZE_MIN)		&&  \
	((pkg)->size <= BTP_PKG_SIZE_MAX)		&&  \
	true)

#define BTP_CHECK_ARG(arg) \
	(((arg) != NULL)				&&  \
	((arg)->magic == BTP_ARG_MAGIC)			&&  \
	((arg)->size >= BTP_ARG_SIZE_MIN)		&&  \
	((arg)->size <= BTP_ARG_SIZE_MAX)		&&  \
	((arg)->nsize >= BTP_ARG_NAME_SIZE_MIN)		&&  \
	((arg)->nsize <= BTP_ARG_NAME_SIZE_MAX)		&&  \
	((arg)->vsize >= BTP_VALUE_SIZE_MIN)		&&  \
	((arg)->vsize <= BTP_VALUE_SIZE_MAX)		&&  \
	((arg)->name != NULL)				&&  \
	((arg)->name[0] != '\0')			&&  \
	true)

#define BTP_CHECK_VALUE(value) \
	(((value) != NULL)				&&  \
	((value)->magic == BTP_VALUE_MAGIC)		&&  \
	true)

#define	BTP_FOREACH(cnt, arg, pkg) \
	for ((cnt) = 0, (arg) = btp_pkg_foreach_arg_first((pkg)); (arg != NULL); (arg) = btp_pkg_foreach_arg_next(arg), (cnt)++)

		
/* BTP functions */
int btp_init(int fd, btp_t **btpp);
void btp_fini(btp_t *btp);

int btp_snd(btp_t *btp, btp_pkg_t *pkg);
int btp_rcv(btp_t *btp, btp_pkg_t **pkgp);

int btp_snd_file(btp_t *btp, const char *filepath);
int btp_rcv_file(btp_t *btp, const char *filepath, size_t size);

/* BTP_PKG functions */
int btp_pkg_init(btp_pkg_t **pkgp);
void btp_pkg_fini(btp_pkg_t *pkg);

bool btp_pkg_check(btp_pkg_t *pkg);

void btp_pkg_set_error(btp_pkg_t *pkg, uint32_t errcode);
uint32_t btp_pkg_get_error(btp_pkg_t *pkg);

void btp_pkg_set_opcode(btp_pkg_t *pkg, uint32_t opcode);
uint32_t btp_pkg_get_opcode(btp_pkg_t *pkg);

uint32_t btp_pkg_argno(btp_pkg_t *pkg);
uint64_t btp_pkg_size(btp_pkg_t *pkg);

void btp_pkg_set_flag(btp_pkg_t *pkg, uint64_t flag);
void btp_pkg_unset_flag(btp_pkg_t *pkg, uint64_t flag);
bool btp_pkg_has_flag(btp_pkg_t *pkg, uint64_t flag);

int btp_pkg_add(btp_arg_t *arg, btp_pkg_t **pkgp);
int btp_pkg_add_number(const char *name, int64_t value, btp_pkg_t **pkgp);
int btp_pkg_add_string(const char *name, const char *value, btp_pkg_t **pkgp);
int btp_pkg_add_binary(const char *name, unsigned char *value, uint32_t size, btp_pkg_t **pkgp);
int btp_pkg_add_bool(const char *name, bool value, btp_pkg_t **pkgp);

int btp_pkg_arg_by_name(btp_pkg_t *pkg, const char *name, btp_arg_t **argp);

btp_arg_t *btp_pkg_foreach_arg_first(btp_pkg_t *pkg);
btp_arg_t *btp_pkg_foreach_arg_next(btp_arg_t *arg);

/* BTP_ARG functions */
int btp_arg_init(btp_arg_t **argp);
void btp_arg_fini(btp_arg_t *arg);

bool btp_arg_check(btp_arg_t *arg);

int btp_arg_create_number(const char *name, int64_t value, btp_arg_t **argp);
int btp_arg_create_string(const char *name, const char *value, btp_arg_t **argp);
int btp_arg_create_binary(const char *name, unsigned char *value, uint32_t size, btp_arg_t **argp);
int btp_arg_create_bool(const char *name, bool value, btp_arg_t **argp);

int btp_arg_get_number(btp_arg_t *arg, int64_t *valuep);
int btp_arg_get_string(btp_arg_t *arg, char **valuep);
int btp_arg_get_binary(btp_arg_t *arg, uint32_t *size, unsigned char **valuep);
int btp_arg_get_bool(btp_arg_t *arg, bool *valuep);

uint32_t btp_arg_size(btp_arg_t *arg);
uint16_t btp_arg_nsize(btp_arg_t *arg);
uint32_t btp_arg_vsize(btp_arg_t *arg);
uint8_t btp_arg_type(btp_arg_t *arg);
const char *btp_arg_name(btp_arg_t *arg);
btp_value_t *btp_arg_value(btp_arg_t *arg);

/* BTP_VALUE functions */
int btp_value_init(btp_value_t **valuep);
void btp_value_fini(btp_value_t *value);

uint8_t btp_value_type(btp_value_t *value);
uint32_t btp_value_size(btp_value_t *value);

int btp_value_create_number(int64_t number, btp_value_t **valuep);
int btp_value_create_string(const char *string, btp_value_t **valuep);
int btp_value_create_binary(unsigned char *binary, uint32_t size, btp_value_t **valuep);
int btp_value_create_bool(bool value, btp_value_t **valuep);

int64_t btp_value_get_number(btp_value_t *value);
char *btp_value_get_string(btp_value_t *value);
unsigned char *btp_value_get_binary(btp_value_t *value, uint32_t *size);
bool btp_value_get_bool(btp_value_t *value);

#endif  /* !_FSTORAGE_BTP_H_ */
