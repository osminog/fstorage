/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_COMMON_H_
#define _FSTORAGE_COMMON_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "fstorage_str.h"
#include "fstorage_keys.h"
#include "fstorage_errcode.h"

#define	FS_HASH_BASE	60

const char *fs_strerror(int errcode);

/* DIR */
// int fs_dir_path(const char *dkey, const char *basepath, char *dirpathp);
int fs_dir_path_from_hash(const char *hash, const char *basepath, char *dirpathp);
bool fs_dir_exist(const char *dirpath);
int fs_dir_create(const char *dirpath);

/* FILE */
// int fs_file_path(const char *fkey, uint64_t salt, uint8_t base, const char *basepath, char *pathp);
int fs_file_path_from_hash(const char *hash, const char *basepath, char *filepathp);
bool fs_file_exist(const char *filepath);
int fs_file_remove(const char *filepath);
int fs_file_size(const char *filepath, size_t *size);

/* file locking */
void fs_read_lock(int fd);
void fs_write_lock(int fd);
void fs_append_lock(int fd);
void fs_unlock(int fd);

/* DIVERS */
int fs_hash(const char *key, char *hashp);
int fs_setnonblock(int fd);

#endif  /* !_FSTORAGE_COMMON_H_ */
