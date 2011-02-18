/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_impl.h"
#include "fstorage_mime_types.h"
#include "fstorage.h"

/* Fstorage */
int
fstorage_init(const char *backends, unsigned int debug, const char *logfile, fs_ctx_t **ctxp)
{
	int err;
	fs_log_t *log = NULL;
	fs_conf_t *cfg = NULL;
	
	if ((err = fstorage_conf_init(backends, debug, logfile, &cfg)) == ERR_OK) {
		if ((err = fs_log_init((char *)&(cfg->logfile), &log)) == ERR_OK) {
			fs_log_set_level(log, cfg->debug);
			if ((err = fstorage_ctx_init(cfg, log, ctxp)) == ERR_OK) {
				return (ERR_OK);
			}
			fs_log_fini(log);
		}
		fstorage_conf_fini(cfg);
	}
	
	return (ERR_CONFIG);
}

void
fstorage_fini(fs_ctx_t *ctx)
{
	
	if (ctx == NULL)
		return;
	
	fstorage_ctx_fini(ctx);
}



const char *
fstorage_strerror(int errcode)
{
	
	return (fs_strerror(errcode));
}

int
fstorage_hash(const char *key, char *hashp)
{
	
	return (fs_hash(key, hashp));
}

int
fstorage_ns_encode(uint64_t num, uint8_t base, char *ciphertextp)
{
	
	return (fs_ns_encode(num, base, ciphertextp));
}

int
fstorage_ns_decode(const char *ciphertext, uint8_t base, uint64_t *num)
{
	
	return (fs_ns_decode(ciphertext, base, num));
}

int
fstorage_akey_create(uint32_t expire_time, const char *fkey, uint64_t salt, uint8_t base, char *akeyp)
{
	int err;
	uint32_t secret;
	uint16_t id_dir, id_mimetype;
	unsigned int cnt;
	char **tab;
	
	str_split(fkey, "::", &cnt, &tab);
	
	if (cnt == 2) {
		if ((err = fs_fkey_decode(tab[1], salt, base, &id_mimetype, &id_dir, &secret)) == ERR_OK)
			err = fs_akey_encode(expire_time, secret, salt, base, akeyp);
	} else {
		err = ERR_PARAM;
	}
	
	free(tab);
	
	return (err);
}

bool
fstorage_akey_check(const char *akey, const char *fkey, uint64_t salt, uint8_t base)
{
	uint32_t secret;
	uint16_t id_dir, id_mimetype;
	unsigned int cnt;
	char **tab;
	bool result = false;
	
	str_split(fkey, "::", &cnt, &tab);
	
	result = ((cnt == 2) 									&&
		(fs_fkey_decode(tab[1], salt, base, &id_mimetype, &id_dir, &secret) == ERR_OK) 	&&
		(fs_akey_check(akey, secret, salt,  base) == ERR_OK)				&&
		true);
	
	free(tab);
	
	return (result);
}

int
fstorage_uri(const char *filename, const char *fkey, const char *akey, char *urip)
{
	int err, n;
	unsigned int cnt;
	char **tab;
	
	str_split(fkey, "::", &cnt, &tab);
	
	err = (	cnt != 2 				||
		fkey == NULL				||
		fkey[0] == '\0' 			||
		filename == NULL 			||
		filename[0] =='\0' 			||
		(akey != NULL && akey[0] == '\0') 	||
		false) ? ERR_PARAM : ERR_OK;
	
	if (err == ERR_OK) {
		if (akey != NULL)
			n = sprintf(urip, "/%s?fkey=%s&akey=%s", filename, (const char *)tab[1], akey);
		else
			n = sprintf(urip, "/%s?fkey=%s", filename, (const char *)tab[1]);
	}
	
	free(tab);
	
	return (err);
}

/** _FSTORAGE_URL */
static int
_fstorage_url(fs_ctx_t *ctx, const char *filename, const char *fkey, const char *akey, bool https, char *urlp)
{
	int err, n;
	unsigned int cnt, id_backend;
	char **tab;
	char uri[1024];
	
	str_split(fkey, "::", &cnt, &tab);
	
	err = cnt == 2 ? ERR_OK : ERR_PARAM;
	
	if (err == ERR_OK && (err = fstorage_uri(filename, fkey, akey, (char *)&uri)) == ERR_OK) {
		if ((err = fstorage_get_backend_by_name(ctx, (const char *)tab[0], &id_backend)) == ERR_OK) {
			n = sprintf(urlp, "%s://%s%s", https ? "https" : "http", ctx->cfg->backends[id_backend]->host, uri);
		}
	}
	
	free(tab);
	
	return (err);
}

int
fstorage_http_url(fs_ctx_t *ctx, const char *filename, const char *fkey, const char *akey, char *urlp)
{
	
	return (_fstorage_url(ctx, filename, fkey, akey, false, urlp));
}

int
fstorage_https_url(fs_ctx_t *ctx, const char *filename, const char *fkey, const char *akey, char *urlp)
{
	
	return (_fstorage_url(ctx, filename, fkey, akey, true, urlp));
}

/** OP_HANDSHAKE */
static int
fstorage_op_handshake(fs_ctx_t *ctx, btp_t *btp)
{
	int err;
	btp_pkg_t *pkg_in = NULL, *pkg_out = NULL;
	
	/* Receiving pkg */
	if ((err = btp_rcv(btp, &pkg_in)) != ERR_OK) {
		fs_error(ctx->log, "Couldnot receive package.");
		return (err);
	}
	
	btp_pkg_fini(pkg_in);
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_HANDSHAKE);
	assert(ERR_OK == btp_pkg_add_string("handshake", "Hello!", &pkg_out));
	
	err = btp_snd(btp, pkg_out);	
	
	btp_pkg_fini(pkg_out);
	
	return (err);
}

/** OP_FILE_SEND */
static int
fstorage_op_file_send(fs_ctx_t *ctx, btp_t *btp, const char *key, const char *filepath)
{
	int err;
	size_t size;
	btp_arg_t *arg;
	btp_pkg_t *pkg_in = NULL, *pkg_out = NULL;
	char *hash, tmp[16];
	
	if ((err = fs_file_size(filepath, &size)) != ERR_OK)
		return (err);
	
	if ((err = fs_hash(key, (char *)&tmp)) != ERR_OK)
		return (err);
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_FILE_SEND);
	assert(ERR_OK == btp_pkg_add_number("size", (int64_t)size, &pkg_out));
	assert(ERR_OK == btp_pkg_add_string("hash", (char *)tmp, &pkg_out));
	assert(ERR_OK == btp_pkg_add_number("mimetype", fs_mimetype_get_id_from_filename(filepath), &pkg_out));
	
	/* Sending pkg */
	err = btp_snd(btp, pkg_out);
	
	btp_pkg_fini(pkg_out);
	
	if (err != ERR_OK)
		return (err);
	
	/* Receiving pkg and extract arguments */
	if ((err = btp_rcv(btp, &pkg_in)) != ERR_OK)
		return (err);
	
	err = btp_pkg_get_error(pkg_in);
	
	if (err == ERR_OK && !(btp_pkg_arg_by_name(pkg_in, "hash", &arg) == ERR_OK && btp_arg_get_string(arg, &hash) == ERR_OK))
		err = ERR_CORRUPTED;
	
	btp_pkg_fini(pkg_in);
	
	if (err != ERR_OK)
		return (err);
	
	/* Sending file */
	btp_snd_file(btp, filepath);
	
	/* Receiving pkg */
	if ((err = btp_rcv(btp, &pkg_in)) != ERR_OK)
		return (err);
	
	err = btp_pkg_get_error(pkg_in);
	
	btp_pkg_fini(pkg_in);
	
	return (err);
}

/** OP_FILE_RECV */ 
static int
fstorage_op_file_recv(fs_ctx_t *ctx, btp_t *btp, const char *key, const char *filepath, size_t *sizep)
{
	int err;
	int64_t size;
	btp_arg_t *arg;
	btp_pkg_t *pkg_in = NULL, *pkg_out = NULL;
	char hash[16];
	
	if ((err = fs_hash(key, (char *)&hash)) != ERR_OK)
		return (err);
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_FILE_RECV);
	assert(ERR_OK == btp_pkg_add_string("hash", hash, &pkg_out));
	
	err = btp_snd(btp, pkg_out);
	
	btp_pkg_fini(pkg_out);
	
	if (err != ERR_OK)
		return (err);
	
	/* Receiving pkg */
	if ((err = btp_rcv(btp, &pkg_in)) != ERR_OK)
		return (err);
	
	err = btp_pkg_get_error(pkg_in);
	
	if (err == ERR_OK && !(btp_pkg_arg_by_name(pkg_in, "size", &arg) == ERR_OK && btp_arg_get_number(arg, &size) == ERR_OK))
		err = ERR_CORRUPTED;
	
	btp_pkg_fini(pkg_in);
	
	if (err != ERR_OK)
		return (err);
	
	/* Receiving file */
	err = btp_rcv_file(btp, filepath, size);
	
	if (err != ERR_OK)
		return (err);
	
	*sizep = (size_t)size;
	
	return (ERR_OK);
}

/** OP_FILE_REPLACE */
static int
fstorage_op_file_replace(fs_ctx_t *ctx, btp_t *btp, const char *key, const char *filepath)
{
	int err;
	FILE *fd;
	size_t size;
	btp_pkg_t *pkg_in = NULL, *pkg_out = NULL;
	char hash[16];
	
	if ((err = fs_hash(key, (char *)&hash)) != ERR_OK)
		return (err);
	
	/* Opening file to get file size with tell function */
	if (!(fd = fopen(filepath, "rb")))
		return (ERR_FILESYSTEM);
	
	if ((err = fs_file_size(filepath, &size)) != ERR_OK)
		return (err);
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_FILE_REPLACE);
	assert(ERR_OK == btp_pkg_add_string("hash", hash, &pkg_out));
	assert(ERR_OK == btp_pkg_add_number("size", (int64_t)size, &pkg_out));
	
	/* Sending pkg */
	err = btp_snd(btp, pkg_out);
	
	btp_pkg_fini(pkg_out);
	
	if (err != ERR_OK)
		return (err);
	
	/* Receiving pkg and extract arguments */
	if ((err = btp_rcv(btp, &pkg_in)) != ERR_OK)
		return (err);
	
	err = btp_pkg_get_error(pkg_in);
	
	btp_pkg_fini(pkg_in);
	
	if (err != ERR_OK)
		return (err);
	
	/* Sending file */
	btp_snd_file(btp, filepath);
	
	/* Receiving pkg */
	if ((err = btp_rcv(btp, &pkg_in)) != ERR_OK)
		return (err);
	
	err = btp_pkg_get_error(pkg_in);
	
	btp_pkg_fini(pkg_in);
	
	return (err);
}

/** OP_REMOVE */
static int
fstorage_op_remove(fs_ctx_t *ctx, btp_t *btp, const char *key)
{
	int err;
	btp_pkg_t *pkg_in = NULL, *pkg_out = NULL;
	char hash[16];
	
	if ((err = fs_hash(key, (char *)&hash)) != ERR_OK)
		return (err);
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_REMOVE);
	assert(ERR_OK == btp_pkg_add_string("hash", hash, &pkg_out));
	
	/* Sending pkg */
	err = btp_snd(btp, pkg_out);
	
	btp_pkg_fini(pkg_out);
	
	if (err != ERR_OK)
		return (err);
	
	/* Receiving pkg and checking errcode */
	if ((err = btp_rcv(btp, &pkg_in)) != ERR_OK)
		return (err);
	
	err = btp_pkg_get_error(pkg_in);
	
	btp_pkg_fini(pkg_in);
	
	return (err);
}

/** GETING CONNECTION */

static int
fstorage_get_conn_by_backend(fs_ctx_t *ctx, unsigned int id_backend, btp_t **btpp)
{
	int err, fd;
	struct sockaddr_in sa;
	btp_t *btp;
	char *host = ctx->cfg->backends[id_backend]->host;
	unsigned int port = ctx->cfg->backends[id_backend]->port;
	
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return (ERR_SOCKET);
	
	memset(&sa, 0, sizeof(sa));
	
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(host);
	
	if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		close(fd);
		return (ERR_CONNECT);
	}
	
	if ((err = btp_init(fd, &btp)) != ERR_OK)
		return (err);
	
	write(fd, "FSTP\r\n", strlen("FSTP\r\n"));
	
	if ((err = fstorage_op_handshake(ctx, btp)) !=  ERR_OK)
		return (err);
	
	*btpp = btp;
	
	return (ERR_OK);
}

static int
fstorage_get_conn_shuffle(fs_ctx_t *ctx, unsigned int *id_backendp, btp_t **btpp)
{
	btp_t *btp = NULL;
	unsigned int i, x, id, ids[FS_BACKENDS_LIMIT];
	bool isin;
	
	memset(ids, 0, FS_BACKENDS_LIMIT);
	srand(time(NULL));
	x = 0;
	while(true) {
		
		id = rand() % ctx->cfg->backends_cnt;
		
		isin = false;
		i = 0;
		while (i < x) {
			if (ids[i] == id) {
				isin = true;
				break;
			}
			i++;
		}
		
		if (!isin && (x < ctx->cfg->backends_cnt)) {
			if (fstorage_get_conn_by_backend(ctx, id, &btp) != ERR_OK) {
				ids[x] = id;
				x++;
			} else {
				break;
			}
		} else if (x < ctx->cfg->backends_cnt) {
			//DO NOTHING
		} else {
			return (ERR_CONNECT);
		}
	}
	
	*btpp = btp;
	*id_backendp = id;
	
	return (ERR_OK);
}

/** FILE */

int
fstorage_file_send(fs_ctx_t *ctx, const char *key, const char *filepath)
{
	int err, n;
	unsigned int id;
	btp_t *btp;
	
	if (!fs_file_exist(filepath))
		return (ERR_FILESYSTEM);
	
	if ((err = fstorage_get_conn_shuffle(ctx, &id, &btp)) != ERR_OK)
		return (err);
	
	err = fstorage_op_file_send(ctx, btp, key, filepath);
	
	btp_fini(btp);
	
	return (err);
}

int
fstorage_file_recv(fs_ctx_t *ctx, const char *key, const char *filepath, size_t *sizep)
{
	int err;
	unsigned int id;
	btp_t *btp;
	
	if ((err = fstorage_get_conn_shuffle(ctx, &id, &btp)) != ERR_OK)
		return (err);
	
	err = fstorage_op_file_recv(ctx, btp, key, filepath, sizep);
	
	btp_fini(btp);
	
	return (err);
}

int
fstorage_file_replace(fs_ctx_t *ctx, const char *key, const char *filepath)
{
	int err;
	unsigned int id;
	btp_t *btp;
		
	if ((err = fstorage_get_conn_shuffle(ctx, &id, &btp)) != ERR_OK)
		return (err);
	
	err = fstorage_op_file_replace(ctx, btp, key, filepath);
	
	btp_fini(btp);
	
	return (err);
}

int
fstorage_file_remove(fs_ctx_t *ctx, const char *key)
{
	int err;
	unsigned int id;
	btp_t *btp;
		
	if ((err = fstorage_get_conn_shuffle(ctx, &id, &btp)) != ERR_OK)
		return (err);
	
	err = fstorage_op_remove(ctx, btp, key);
	
	btp_fini(btp);
	
	return (err);
}
