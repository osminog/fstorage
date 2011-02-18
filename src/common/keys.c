/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_keys.h"


/**
 * Random cardinal number.
 * @param max_num: range <1, max_num>
 * @return:
 *		cardinal.
 */
int
fs_numgen(unsigned int num_max)
{
	
	srand(time(NULL));
	
	return  (rand() % num_max) + 1;
}

/**
 * Encode id_dir to dkey.
 * @param id_dir: directory identifier as cardinal
 * @param salt: secret number as cardinal
 * @param base: numeric systems base number as cardinal <1, 62>
 * @param dkey: encoded directory key as string
 * @return:
 *		ERR_* as integer
 */
int
fs_dkey_encode(uint16_t id_dir, uint64_t salt, uint8_t base, char *dkeyp)
{
	int err;
	
	if (id_dir < 1)
		return (ERR_PARAM);
	
	if ((err = fs_ns_encode(id_dir ^ salt, base, dkeyp)) != ERR_OK)
		return (err);
	
	return (ERR_OK);
}

/**
 * Decode dkey to id_dir.
 * @param dkey: encoded directory key as string
 * @param salt: secret number as cardinal
 * @param base: numeric systems base number as cardinal <1, 62>
 * @param id_dir: directory identifier as cardinal
 * @return:
 *		ERR_* as integer
 */
int
fs_dkey_decode(const char *dkey, uint64_t salt, uint8_t base, uint16_t *id_dirp)
{
	int err;
	uint64_t tmp;
	
	if (dkey == NULL || dkey == '\0')
		return (ERR_PARAM);
	
	if ((err = fs_ns_decode(dkey, base, &tmp)) != ERR_OK)
		return (err);
	
	*id_dirp = tmp ^ salt;
	
	return (ERR_OK);
}

/**
 * Encode id_mimetype, id_dir to fkey.
 * @param id_mimetype: mime-type identifier as cardinal
 * @param id_dir: directory identifier as cardinal
 * @param salt: secret number as cardinal
 * @param base: numeric systems base number as cardinal <1, 62>
 * @param fkey: encoded file key as string
 * @return:
 *		ERR_* as integer
 */
int
fs_fkey_encode(uint16_t id_mimetype, uint16_t id_dir, uint64_t salt, uint8_t base, char *fkeyp)
{
	int r = fs_numgen(1024*128);
	
	return (fs_fkey_encode_r(id_mimetype, id_dir, salt, base, r, fkeyp));
}

int
fs_fkey_encode_r(uint16_t id_mimetype, uint16_t id_dir, uint64_t salt, uint8_t base, int r, char *fkeyp)
{
	int err, n;
	uint64_t key;
	char tmp[64] = "", rs[16] = "";
	
	memset(&tmp, 0, 64);
	memset(&rs, 0, 16);
	
	n = sprintf(rs, "%u", r);
	
	n = sprintf(tmp, "%lu%s%03d%05d", strlen(rs), rs, id_mimetype, id_dir);
	
	sscanf(tmp, "%lu", &key);
	
	if ((err = fs_ns_encode(key ^ salt, base, fkeyp)) != ERR_OK)
		return (err);
	
	return (ERR_OK);
}

/**
 * Decode fkey to id_mimetype, id_dir. 
 * @param fkey: encoded file key as string
 * @param salt: secret number as cardinal
 * @param base: numeric systems base number as cardinal <1, 62>
 * @param id_mimetype: mime-type identifier as cardinal
 * @param id_dir: directory identifier as cardinal
 * @return:
 *		ERR_* as integer
 */
int
fs_fkey_decode(const char *fkey, uint64_t salt, uint8_t base, uint16_t *id_mimetypep, uint16_t *id_dirp, uint32_t *secretp)
{
	int n, err, id_mimetype, id_dir;
	uint32_t r;
	uint64_t key;
	char tmp[64], buff[16], *ptr;
	
	memset(&tmp, 0, 64);
	
	if (fkey == NULL || fkey == '\0')
		return (ERR_PARAM);
	
	if ((err = fs_ns_decode(fkey, base, &key)) != ERR_OK)
		return (err);
	
	key = key ^ salt;
	n = sprintf(tmp, "%lu", key);
	r = (int)tmp[0] - 48;
	
	if ((r + 9) != strlen(tmp))
		return (ERR_CORRUPTED);
	
	ptr = tmp;
	
	ptr += 1;
	memset(&buff, 0, 16);
	memcpy(buff, ptr, r);
	sscanf(buff, "%u", &r);
	
	ptr += strlen(buff);
	memset(&buff, 0, 16);
	memcpy(buff, ptr, 3);
	sscanf(buff, "%d", &id_mimetype);
	
	ptr += 3;
	memset(&buff, 0, 16);
	memcpy(buff, ptr, 5);
	sscanf(buff, "%d", &id_dir);
	
	*id_mimetypep = (uint16_t)id_mimetype;
	*id_dirp = (uint16_t)id_dir;
	*secretp = r;
	
	return (ERR_OK);
}

int
fs_akey_encode(uint32_t expire_time, uint32_t secret, uint64_t salt, uint8_t base, char *akeyp)
{
	int n, err;
	time_t t;
	uint64_t key;
	char tmp[32];
	
	time(&t);
	memset(&tmp, 0, 32);
	
	n = sprintf(tmp, "%d%ld", secret, (t + expire_time));
	sscanf(tmp, "%lu", &key);
	
	if ((err = fs_ns_encode(key ^ salt, base, akeyp)) != ERR_OK)
		return (err);
	
	return (ERR_OK);
}

int
fs_akey_decode(const char *akey, uint32_t secret, uint64_t salt, uint8_t base, uint32_t *expire_timep)
{
	int n, err;
	uint64_t key;
	unsigned int secret_tmp;
	time_t t, act;
	char tmp[32], buff[16], *ptr;
	
	time(&act);
	
	if ((err = fs_ns_decode(akey, base, &key)) != ERR_OK)
		return (err);
	
	key = key ^ salt;
	
	memset(&tmp, 0, 32);
	n = sprintf(tmp, "%lu", key);
	
	if (strlen(tmp) <= 10)
		return (ERR_CORRUPTED);
	
	ptr = tmp;
	memset(&buff, 0, 16);
	memcpy(buff, ptr, strlen(ptr) - 10);
	sscanf(buff, "%u", &secret_tmp);
	
	ptr += strlen(ptr) - 10;
	memset(&buff, 0, 16);
	memcpy(buff, ptr, 10);
	sscanf(buff, "%ld", &t);
	
	if (secret_tmp != secret)
		return (ERR_CORRUPTED);
	
	*expire_timep = (t - act) > 0 ? t - act : 0;
	
	return (ERR_OK);
}

int
fs_akey_check(const char *akey, uint32_t secret, uint64_t salt, uint8_t base)
{
	int err;
	uint32_t expire_time;
	
	if ((err = fs_akey_decode(akey, secret, salt, base, &expire_time)) != ERR_OK)
		return (err);
	
	if (expire_time == 0)
		return (ERR_EXPIRED);
	
	return (ERR_OK);
}

