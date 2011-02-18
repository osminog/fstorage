/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_KEYS_H_
#define _FSTORAGE_KEYS_H_

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "fstorage_ns.h"
#include "fstorage_str.h"
#include "fstorage_errcode.h"

int fs_numgen(unsigned int num_max);

int fs_dkey_encode(uint16_t id_dir, uint64_t salt, uint8_t base, char *dkeyp);
int fs_dkey_decode(const char *dkey, uint64_t salt, uint8_t base, uint16_t *id_dirp);

int fs_fkey_encode(uint16_t id_mimetype, uint16_t id_dir, uint64_t salt, uint8_t base, char *fkeyp);
int fs_fkey_encode_r(uint16_t id_mimetype, uint16_t id_dir, uint64_t salt, uint8_t base, int r, char *fkeyp);
int fs_fkey_decode(const char *fkey, uint64_t salt, uint8_t base, uint16_t *id_mimetypep, uint16_t *id_dirp, uint32_t *secretp);

int fs_akey_encode(uint32_t expire_time, uint32_t secret, uint64_t salt, uint8_t base, char *akeyp);
int fs_akey_decode(const char *akey, uint32_t secret, uint64_t salt, uint8_t base, uint32_t *expire_timep);

int fs_akey_check(const char *akey, uint32_t secret, uint64_t salt, uint8_t base);

#endif  /* !_FSTORAGE_KEYS_H_ */

