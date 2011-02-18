/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl> Brendan Smith <bvsmith1986@gmail.com>
 */

#include "fstoraged_hdr.h"
#include "fstoraged_fstp.h"


int
fsd_fstp_snd(fsd_session_t *session, btp_pkg_t *pkg)
{
	
	fs_log_pkg(session->log, pkg, FS_OUT);
	
	return btp_snd(session->btp, pkg);
}

int
fsd_fstp_rcv(fsd_session_t *session, btp_pkg_t **pkgp)
{
	int err;
	
	if ((err = btp_rcv(session->btp, pkgp)) == ERR_OK)
		fs_log_pkg(session->log, *pkgp, FS_IN);
	
	return (err);
}

int
fsd_fstp_handler(fsd_session_t *session, const char *headline)
{
	int err, ret;
	uint16_t opcode;
	btp_pkg_t *pkg_in = NULL, *pkg_out = NULL;
	
	if((ret = fsd_fstp_handshake(session)) == ERR_OK)
		fs_debug(session->log, "Client handshaked.");
	else
		fs_warn(session->log, "Handshake failed with err=%d, %s", ret, fs_strerror(ret));
	
	while (ret == ERR_OK) {

		if ((ret = fsd_fstp_rcv(session, &pkg_in)) == ERR_OK ) {
			
			opcode = btp_pkg_get_opcode(pkg_in);
			
			switch(opcode) {
			
			case OP_REMOVE:
				fs_debug(session->log, "OP_REMOVE (removing file from filesystem)");
				/* Receiving file from client */
				if ((err = fsd_fstp_file_remove(session, pkg_in)) != ERR_OK)
					fs_error(session->log, "Operation 'OP_REMOVE' has failed with error '%s'", fs_strerror(err));
				break;
				
			case OP_FILE_SEND:
				fs_debug(session->log, "OP_FILE_SEND (receiving file from client)");
				/* Receiving file from client */
				if ((err = fsd_fstp_file_send(session, pkg_in)) != ERR_OK)
					fs_error(session->log, "Operation 'OP_FILE_SEND' has failed with error '%s'", fs_strerror(err));
				break;
			
			case OP_FILE_RECV:
				fs_debug(session->log, "OP_FILE_RECV (sending file do client)");
				/* Sending file to client */
				if ((err = fsd_fstp_file_recv(session, pkg_in)) != ERR_OK)
					fs_error(session->log, "Operation 'OP_FILE_RECV' has failed with error '%s'", fs_strerror(err));
				break;
				
			case OP_FILE_REPLACE:
				fs_debug(session->log, "OP_FILE_REPLACE (receiving new and replacing old file)");
				/* Receiving and replacing existing file (atom) from client */
				if ((err = fsd_fstp_file_replace(session, pkg_in)) != ERR_OK)
					fs_error(session->log, "Operation 'OP_FILE_REPLACE' has failed with error '%s'", fs_strerror(err));
				break;
				
			case OP_NONE:
			default:
				fs_debug(session->log, "OP_NONE (unknown operation code)");
				fs_warn(session->log, "Unknown opcode = %d.", opcode);
				if ((err = fsd_fstp_none(session, pkg_in)) != ERR_OK)
					fs_error(session->log, "Operation 'OP_NONE' has failed with error '%d'", fs_strerror(err));
				break;
			}

			if (err != ERR_OK) {
				
				fs_debug(session->log, "Operation status: NOT OK");
				
				/* Creating pkg to snd */
				assert(ERR_OK == btp_pkg_init(&pkg_out));
				btp_pkg_set_opcode(pkg_out, opcode);
				btp_pkg_set_error(pkg_out, err);
				
				if (fsd_fstp_snd(session, pkg_out) != ERR_OK)
					fs_warn(session->log, "Couldn't send ERROR to client");
				
				btp_pkg_fini(pkg_out);
			}

			btp_pkg_fini(pkg_in);
		}		
	}
	
	return (ERR_OK);
}

int
fsd_fstp_none(fsd_session_t* session, btp_pkg_t *pkg_in)
{
	int err;
	btp_pkg_t *pkg_out;
	
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, btp_pkg_get_opcode(pkg_in));
	btp_pkg_set_error(pkg_out, ERR_OPCODE);
	assert(ERR_OK == btp_pkg_add_string("message", "Unknown operation code", &pkg_out));
	
	if ((err = fsd_fstp_snd(session, pkg_out)) != ERR_OK) {
		btp_pkg_fini(pkg_out);
		return (err);
	}
	
	return (ERR_OK);
}

int
fsd_fstp_handshake(fsd_session_t *session)
{
	int err;
	btp_pkg_t *pkg_in = NULL, *pkg_out = NULL;
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_HANDSHAKE);
	assert(ERR_OK == btp_pkg_add_string("handshake", "Hello!", &pkg_out));
	
	if ((err = fsd_fstp_snd(session, pkg_out)) != ERR_OK) {
		btp_pkg_fini(pkg_out);
		return (err);
	}
	
	btp_pkg_fini(pkg_out);
	
	/* Receiving pkg */
	if ((err = fsd_fstp_rcv(session, &pkg_in)) != ERR_OK) {
		return (err);
	}
	
	btp_pkg_fini(pkg_in);
	
	/* Changing session state on HANDSHAKED */
	fsd_session_set_state(session, FS_STATE_HANDSHAKED);
	
	fs_debug(session->log, "SID = %lu", session->id);
	
	return (ERR_OK);
}

int
fsd_fstp_file_send(fsd_session_t *session, btp_pkg_t *pkg_in)
{
	int err, n;
	btp_arg_t *arg;
	btp_pkg_t *pkg_out = NULL;
	int64_t size, mimetype;
	char *hash, dirpath[256], filepath[256], hdrpath[256];
	fsd_hdr_t *hdr;
	
	if (!(btp_pkg_arg_by_name(pkg_in, "size", &arg) == ERR_OK && btp_arg_get_number(arg, &size) == ERR_OK))
		return (ERR_CORRUPTED);
	
	if (!(btp_pkg_arg_by_name(pkg_in, "hash", &arg) == ERR_OK && btp_arg_get_string(arg, &hash) == ERR_OK))
		return (ERR_CORRUPTED);
	
	if (!(btp_pkg_arg_by_name(pkg_in, "mimetype", &arg) == ERR_OK && btp_arg_get_number(arg, &mimetype) == ERR_OK))
		return (ERR_CORRUPTED);
	
	if ((err = fs_file_path_from_hash(hash, session->cfg->path, (char *)&filepath)) != ERR_OK)
		return (err);
	
	n = snprintf((char *)hdrpath, sizeof(hdrpath), "%s.hdr", (const char *)filepath);
	
	/* checking if header file exist */
	if (fs_file_exist(hdrpath)) {
		fs_debug(session->log, "Header file for Hash = %s exists", hash);
		return (ERR_KEY_CONFLICT);
	}
	
	err = ERR_OK;
		
	/* Receiving file */
	if (( err = fs_dir_path_from_hash(hash, session->cfg->path, (char *)&dirpath)) != ERR_OK)
		return (err);
	
	if (!fs_dir_exist(dirpath))
		if ((err = fs_dir_create(dirpath)) != ERR_OK)
			return (err);
	
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_FILE_SEND);
	assert(ERR_OK == btp_pkg_add_string("hash", hash, &pkg_out));

	if ((err = fsd_fstp_snd(session, pkg_out)) != ERR_OK)
		fs_warn(session->log, "Cannot send package to client.");

	btp_pkg_fini(pkg_out);
	
	if (err != ERR_OK)
		return (err);
	
	fs_debug(session->log, "Receiving file...");
	
	/* Receiving file */
	if ((err = btp_rcv_file(session->btp, filepath, size)) != ERR_OK) {
		fs_error(session->log, "Operation status: NOT OK");
		return (err);
	}
	
	/* Saving HEADER file */
	fs_warn(session->log, "Creating and saving HEADER file (%s)", (const char *)hdrpath);
	
	if ((err = fsd_hdr_file_create(FSD_TYPE_FILE, 0, (uint16_t)mimetype, (uint32_t)size, 0, 0, &hdr)) != ERR_OK)
		return (err);
	
	if ((err = fsd_hdr_write(hdr, hdrpath)) != ERR_OK)
		fs_warn(session->log, "Cannot write hdr (%s)", (const char *)hdrpath);
	
	fsd_hdr_fini(hdr);
	
	if (err != ERR_OK)
		return (err);
	
	fs_debug(session->log, "Operation status: OK");
	
	/* Sending confirmation */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_FILE_SEND);
	
	if ((err = fsd_fstp_snd(session, pkg_out)) != ERR_OK)
		fs_warn(session->log, "Cannot send package to client.");
	
	btp_pkg_fini(pkg_out);
	
	return (err);
}

int
fsd_fstp_file_recv(fsd_session_t *session, btp_pkg_t *pkg_in)
{
	int err, n;
	FILE *fd;
	btp_arg_t *arg;
	btp_pkg_t *pkg_out = NULL;
	size_t size;
	char *hash, filepath[256], hdrpath[256];
	fsd_hdr_t *hdr;
	
	if (!(btp_pkg_arg_by_name(pkg_in, "hash", &arg) == ERR_OK && btp_arg_get_string(arg, &hash) == ERR_OK))
		return (ERR_CORRUPTED);
	
	fs_debug(session->log, "hash = %s", hash);
	
	if ((err = fs_file_path_from_hash(hash, session->cfg->path, (char *)&filepath)) != ERR_OK)
		return (err);
	
	n = snprintf((char *)hdrpath, sizeof(hdrpath), "%s.hdr", (const char *)filepath);
		
	/* checking if header file exist */
	if (!fs_file_exist(hdrpath)) {
		fs_debug(session->log, "Header file (%s) for Hash = %s NOT exists", hdrpath, hash);
		return (ERR_DOES_NOT_EXIST);
	}
	
	/* checking if content file exist */
	if (!fs_file_exist(filepath)) {
		fs_debug(session->log, "Content file (%s) for Hash = %s does not exist '%s'", filepath, hash);
		return (ERR_DOES_NOT_EXIST);
	}
	
	fs_debug(session->log, "Reading header file");
	
	if ((err = fsd_hdr_read(hdrpath, &hdr)) != ERR_OK) {
		fs_debug(session->log, "Could not read header file (%s)", hdrpath);
		return (err);
	}
	
	size = fsd_hdr_get_size(hdr);
	
	fs_debug(session->log, "file size = %d", size);
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_FILE_RECV);
	assert(ERR_OK == btp_pkg_add_number("size", (int64_t)size, &pkg_out));
	
	if ((err = fsd_fstp_snd(session, pkg_out)) != ERR_OK)
		fs_warn(session->log, "Cannot send package to client.");
	
	btp_pkg_fini(pkg_out);
	
	if (err != ERR_OK)
		return (err);
	
	fs_debug(session->log, "Sending file...");
	
	/* Sending file */
	if ((err = btp_snd_file(session->btp, filepath)) != ERR_OK)
		return (err);
	
	fs_debug(session->log, "Operation status: OK");
	
	return (ERR_OK);
}

int
fsd_fstp_file_replace(fsd_session_t *session, btp_pkg_t *pkg_in)
{
	int err;
	FILE *fd;
	btp_arg_t *arg;
	btp_pkg_t *pkg_out = NULL;
	size_t size;
	char *hash, filepath[256];
	
	if (!(btp_pkg_arg_by_name(pkg_in, "hash", &arg) == ERR_OK && btp_arg_get_string(arg, &hash) == ERR_OK))
		return (ERR_CORRUPTED);
	
	if (!(btp_pkg_arg_by_name(pkg_in, "size", &arg) == ERR_OK && btp_arg_get_number(arg, (int64_t *)&size) == ERR_OK))
		return (ERR_CORRUPTED);
	
	fs_debug(session->log, "hash = %s", hash);
	
	if ((err = fs_file_path_from_hash(hash, session->cfg->path, (char *)&filepath)) != ERR_OK)
		return (err);
				
	if (!fs_file_exist(filepath)) {
		fs_debug(session->log, "File does not exist '%s'", filepath);
		return (ERR_DOES_NOT_EXIST);
	}
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_FILE_REPLACE);
	assert(ERR_OK == btp_pkg_add_string("hash", hash, &pkg_out));
	assert(ERR_OK == btp_pkg_add_number("size", (int64_t)size, &pkg_out));
	
	if ((err = fsd_fstp_snd(session, pkg_out)) != ERR_OK)
		fs_warn(session->log, "Cannot send package to client.");
	
	btp_pkg_fini(pkg_out);
	
	if (err != ERR_OK)
		return (err);
	
	fs_debug(session->log, "Receiving file...");
	
	/* Receiving file for replace old version */
	if ((err = btp_rcv_file(session->btp, filepath, size)) != ERR_OK) {
		fs_error(session->log, "Operation status: NOT OK");
		return (err);
	}
	
	fs_debug(session->log, "Operation status: OK");
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_FILE_REPLACE);
	
	if ((err = fsd_fstp_snd(session, pkg_out)) != ERR_OK)
		fs_warn(session->log, "Cannot send package to client.");
	
	btp_pkg_fini(pkg_out);
	
	return (err);
}

int
fsd_fstp_file_remove(fsd_session_t *session, btp_pkg_t *pkg_in)
{
	int err, n;
	btp_arg_t *arg;
	btp_pkg_t *pkg_out = NULL;
	size_t size;
	char *hash, filepath[256], hdrpath[256];;
	
	if (!(btp_pkg_arg_by_name(pkg_in, "hash", &arg) == ERR_OK && btp_arg_get_string(arg, &hash) == ERR_OK))
		return (ERR_CORRUPTED);
	
	fs_debug(session->log, "hash = %s", hash);
	
	if ((err = fs_file_path_from_hash(hash, session->cfg->path, (char *)&filepath)) != ERR_OK)
		return (err);
	
	n = snprintf((char *)hdrpath, sizeof(hdrpath), "%s.hdr", (const char *)filepath);
	
	/* checking if header file exist */
	if (!fs_file_exist(hdrpath)) {
		fs_debug(session->log, "Header file (%s) for Hash = %s NOT exists", hdrpath, hash);
		return (ERR_DOES_NOT_EXIST);
	}
	
	if ((err = fs_file_remove(hdrpath)) != ERR_OK)
		return (err);
				
	if ((err = fs_file_remove(filepath)) != ERR_OK)
		return (err);
	
	/* Creating pkg to snd */
	assert(ERR_OK == btp_pkg_init(&pkg_out));
	btp_pkg_set_opcode(pkg_out, OP_REMOVE);
	assert(ERR_OK == btp_pkg_add_string("hash", hash, &pkg_out));
	
	if ((err = fsd_fstp_snd(session, pkg_out)) != ERR_OK)
		fs_warn(session->log, "Cannot send package to client.");
	
	btp_pkg_fini(pkg_out);
	
	return (err);
}
