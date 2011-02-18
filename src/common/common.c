/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include <fcntl.h>

#include "fstorage_common.h"
#include "fstorage_ns.h"
#include "fstorage_crc.h"


/**
 * Getting dir path. 
 * @param dkey: encoded dir key as string
 * @param basepath: base path as string
 * @param path: abspath to dir
 * @return:
 *		ERR_* as integer

int
fs_dir_path(const char *dkey, const char *basepath, char *pathp)
{
	int n;
	char base[256];
	
	if (dkey == NULL || dkey == '\0' || basepath == NULL || basepath == '\0')
		return (ERR_PARAM);
	
	strcpy(base, basepath);
	while(base[strlen(base) - 1] == '/')
		base[strlen(base) - 1] = '\0';
	
	n = sprintf(pathp, "%s/%s", base, dkey);
	
	return (ERR_OK);
}
*/

int
fs_dir_path_from_hash(const char *hash, const char *basepath, char *dirpathp)
{
	int n, err;
	uint64_t L;
	uint8_t D;
	
	if (hash == NULL || hash == '\0' || basepath == NULL || basepath == '\0')
		return (ERR_PARAM);
	
	if ((err = fs_ns_decode(hash, FS_HASH_BASE, &L)) != ERR_OK)
		return (err);
	
	D = L % 256;
	
	n = sprintf(dirpathp, "%s/%u", basepath, D);
	
	return (ERR_OK);
}

/**
 * Checking that directory path exists.
 * @param dirpath: path to directory
 * @return:
 *		boolean
 */
bool
fs_dir_exist(const char *dirpath)
{
	struct stat st;
	
	return (stat(dirpath, &st) == 0);
}

/**
 * Creating directory path.
 * @param dirpath: path to new directory
 * @return:
 *		boolean
 */
int
fs_dir_create(const char *dirpath)
{
	int err;
	
	return ((err = mkdir(dirpath, S_IRWXG | S_IRWXU | S_IRWXO)) != ERR_OK ? ERR_FILESYSTEM : ERR_OK); 
}

/**
 * Getting file path.
 * @param fkey: encoded file key as string
 * @param salt: secret number as cardinal
 * @param base: numeric systems base number as cardinal <1, 62>
 * @param baspath: mime-type identifier as cardinal
 * @param path: abspath do file
 * @return:
 *		ERR_* as integer

int
fs_file_path(const char *fkey, uint64_t salt, uint8_t base, const char *basepath, char *pathp)
{
	int n, err;
	char dirpath[256], dkey[256];
	uint16_t id_mimetype, id_dir;
	uint32_t secret;
	
	if (fkey == NULL || fkey == '\0' || basepath == NULL || basepath == '\0')
		return (ERR_PARAM);
	
	if ((err = fs_fkey_decode(fkey, salt, base, &id_mimetype, &id_dir, &secret)) != ERR_OK)
		return (err);
	
	if ((err = fs_dkey_encode(id_dir, salt, base, (char *)&dkey)) != ERR_OK)
		return (err);
	
	if ((err = fs_dir_path(dkey, basepath, (char *)&dirpath)) != ERR_OK)
		return (err);
	
	n = sprintf(pathp, "%s/%s", dirpath, fkey);
	
	return (ERR_OK);
}
*/

int
fs_file_path_from_hash(const char *hash, const char *basepath, char *filepathp)
{
	int n, err;
	uint64_t L;
	uint8_t D;
	
	if (hash == NULL || hash == '\0' || basepath == NULL || basepath == '\0')
		return (ERR_PARAM);
	
	if ((err = fs_ns_decode(hash, FS_HASH_BASE, &L)) != ERR_OK)
		return (err);
	
	D = L % 256;
	
	n = sprintf(filepathp, "%s/%u/%s", basepath, D, hash);
	
	return (ERR_OK);
}

/**
 * Checking that file path exists.
 * @param path: path to file
 * @return:
 *		boolean
 */
bool
fs_file_exist(const char *filepath)
{
	struct stat st;
	
	return (stat(filepath, &st) == 0);
}

int
fs_file_remove(const char *filepath)
{
	
	if (!fs_file_exist(filepath))
		return (ERR_DOES_NOT_EXIST);
	
	if (remove(filepath) != 0)
		return (ERR_FILESYSTEM);
	
	return (ERR_OK);
}

int
fs_file_size(const char *filepath, size_t *size)
{
	FILE *fd;
	
	if(!(fd = fopen(filepath, "rb")))
		return (ERR_FILESYSTEM);
	
	fseek(fd, 0, 2);
	*size = ftell(fd);
	fseek(fd, 0, 0);
	
	fclose(fd);

	return (ERR_OK);
}

/** locker */
static struct flock *
fs_file_lock(short type, short whence) 
{
	static struct flock ret;
	ret.l_type = type;
	ret.l_start = 0;
	ret.l_whence = whence;
	ret.l_len = 0;
	ret.l_pid = getpid();

	return &ret;
}

/** A shared lock on an entire file */
void
fs_read_lock(int fd)
{
	
	fcntl(fd, F_SETLKW, fs_file_lock(F_RDLCK, SEEK_SET));
}

/** An exclusive lock on an entire file */
void
fs_write_lock(int fd)
{

	fcntl(fd, F_SETLKW, fs_file_lock(F_WRLCK, SEEK_SET));
}

/** A lock on the _end_ of a file. Other
processes may access existing records */
void
fs_append_lock(int fd)
{
	
	fcntl(fd, F_SETLKW, fs_file_lock(F_WRLCK, SEEK_END));
}

void
fs_unlock(int fd)
{
	
	fcntl(fd, F_SETLKW, fs_file_lock(F_UNLCK, SEEK_END));
}


int
fs_hash(const char *key, char *hashp)
{
	int err;
	uint64_t crc_hash;
	
	crc_hash = fs_crc(key, strlen(key));
	
	if ((err = fs_ns_encode(crc_hash, FS_HASH_BASE, hashp)) != ERR_OK)
		return (err);
	
	return (ERR_OK);
}

int
fs_setnonblock(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    if (flags < 0)
            return flags;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) 
            return -1;

    return 0;
}

/** Translation error code to message */
const char *
fs_strerror(int errcode)
{
	
	switch(errcode) {
	case ERR_OK: return "Operation succeeded";
	case ERR_MEMORY: return "Out of memory";
	case ERR_FILESYSTEM: return "File does not exist or no permission to open";
	case ERR_INVALID: return "Invalid data";
	case ERR_REGEXP: return "Regexp not valid";
	case ERR_STR_LENGTH: return "Wrong table of character length";
	case ERR_NO_RESULT: return "No result(s)";
	case ERR_TYPE: return "Wrong data/argument type";
	case ERR_SOCKET: return "Socket error";
	case ERR_SETSOCKOPT: return "Setting socket option failed";
	case ERR_BIND: return "Cannot bind address to socket";
	case ERR_LISTEN: return "Error while listening";
	case ERR_CONFIG: return "Config file does not exist or missing variable.";
	case ERR_SETNONBLOCK: return "Error while setting non-blocking";
	case ERR_CONNECT: return "Connection error";
	case ERR_PARAM: return "Wrong params, arguments ora data";
	case ERR_CORRUPTED: return "Data corrupted";
	case ERR_EXPIRED: return "Key has expired";	
	case ERR_FORK: return "Cannot create new process (fork)";
	case ERR_SIZE: return "Wrong size";
	case ERR_OPCODE: return "Unknown operation code";
	case ERR_DOES_NOT_EXIST: return "Not exist!";
	case ERR_KEY_CONFLICT: return "Hash/key conflict";
	
	default:
		return "Unknown error code";
		break;
	}
}

