/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <time.h>

#include <sys/stat.h>
#ifdef HAVE_SYS_SENDFILE_H
#  include <sys/sendfile.h>
#endif /* !HAVE_SYS_SENDFILE_H */
#include <sys/types.h>
#include <sys/socket.h>

#include "fstorage_errcode.h"
#include "fstorage_mime_types.h"
#include "fstoraged_hdr.h"
#include "fstoraged_http.h"

static ssize_t
fsd_sendfile(int ofd, int ifd, off_t *offset, size_t size)
{
    off_t sent_bytes;
    int ret;
	
#if defined(__APPLE__)
	
    sent_bytes = size;
    ret = sendfile(ofd, ifd, *offset, &sent_bytes, NULL, 0);
    if (ret == -1) {
        if (errno == EAGAIN) {
            if (sent_bytes == 0) {
                /* Didn't send anything. Return error with errno == EAGAIN. */
                return -1;
            } else {
                /* We sent some bytes, but they we would block.  Treat this as
                 * success for now. */
                *offset += sent_bytes;
                return sent_bytes;
            }
        } else {
            /* some other error */
            return -1;
        }
    } else if (ret == 0) {
        *offset += size;
        return size;
    } else {
        return -1;
    }
 
#elif defined(__FreeBSD__)
    
    /* According to the manual, this can never partially complete on a
     * socket open for blocking IO. */
    ret = sendfile(ifd, ofd, *offset, size, 0, &sent_bytes, 0);
    if (ret == -1) {
        if (errno == EAGAIN) {
            *offset += sent_bytes;
            return sent_bytes;
        } else {
            return -1;
        }
    } else if (ret == 0) {
        *offset += size;
        return size;
    } else {
        return -1;
    }
#else /* LINUX */
    return sendfile(ofd, ifd, offset, size);
#endif
}

static int
fsd_http_hdr_item_init(fsd_http_hdr_item_t **itemp)
{
	if ((*itemp = malloc(sizeof(fsd_http_hdr_item_t))) == NULL)
		return (ERR_MEMORY);
		
	return (ERR_OK);
}

static void
fsd_http_hdr_item_fini(fsd_http_hdr_item_t *item)
{
	if (item == NULL)
		return;
	
	free(item);
}


static int
fsd_http_hdr_item_create(const char *name, const char *value, fsd_http_hdr_item_t **itemp)
{
	int err;
	
	if (name == NULL || strlen(name) == 0 || strlen(name) > FSD_HTTP_HDR_ITEM_NAME_SIZE_MAX)
		return (ERR_PARAM);
	
	if (value == NULL || strlen(value) == 0 || strlen(value) > FSD_HTTP_HDR_ITEM_VALUE_SIZE_MAX)
		return (ERR_PARAM);
	
	if ((err = fsd_http_hdr_item_init(itemp)) != ERR_OK)
		return (err);
	
	strncpy((char *)(*itemp)->name, name, strlen(name) + 1);
	(*itemp)->name[strlen(name)] = '\0';
	
	strncpy((char *)(*itemp)->value, value, strlen(value) + 1);
	(*itemp)->value[strlen(value)] = '\0';
	
	return (ERR_OK);
}

static int
fsd_http_hdr_set(fsd_http_hdr_item_t *items[], uint16_t cnt, const char *name, const char *value)
{
	int err;
	fsd_http_hdr_item_t *item;
	
	if (name == NULL || strlen(name) == 0 || strlen(name) > FSD_HTTP_HDR_ITEM_NAME_SIZE_MAX)
		return (ERR_PARAM);
	
	if (value == NULL || strlen(value) == 0 || strlen(value) > FSD_HTTP_HDR_ITEM_VALUE_SIZE_MAX)
		return (ERR_PARAM);
	
	/* Trying update attr if exist with name */
	for (unsigned int i = 0; items[i] != NULL; i++) {
		if (items[i] != NULL && items[i]->name != NULL && strncmp(items[i]->name, name, strlen(name)) == 0) {
			strncpy((char *)(items[i]->value), value, strlen(value) + 1);
			return (ERR_OK);
		}
	}
	
	if ((err = fsd_http_hdr_item_create(name, value, &item)) != ERR_OK)
		return (err);
	
	items[cnt] = item;
	
	return (ERR_OK);
}

static const char *
fsd_http_hdr_get(fsd_http_hdr_item_t *items[], const char *name)
{
	
	if (!(name == NULL || strlen(name) == 0 || strlen(name) > FSD_HTTP_HDR_ITEM_NAME_SIZE_MAX)) {
		for (unsigned int i = 0; items[i] != NULL; i++) {
			if (items[i]->name != NULL && strcmp((const char *)(items[i]->name), name) == 0) {
				return ((const char *)(items[i]->value));
			}
		}
	}
	
	return (NULL);
}

int
fsd_http_hdr_init(fsd_http_hdr_t **http_hdrp)
{
	
	if ((*http_hdrp = malloc(sizeof(fsd_http_hdr_t))) == NULL)
		return (ERR_MEMORY);
	
	(*http_hdrp)->attrs_cnt = 0;
	(*http_hdrp)->args_cnt = 0;
	
	memset((*http_hdrp)->attrs, 0, FSD_HTTP_HDR_ATTRS_MAX);
	memset((*http_hdrp)->args, 0, FSD_HTTP_HDR_ARGS_MAX);
	
	return (ERR_OK);
}

void
fsd_http_hdr_fini(fsd_http_hdr_t *http_hdr)
{
	
	if (http_hdr == NULL)
		return;
	
	/*
	for (unsigned int i = 0; http_hdr->attrs[i] != NULL; i++)
		fsd_http_hdr_item_fini(http_hdr->attrs[i]);
	
	for (unsigned int i = 0; http_hdr->args[i] != NULL; i++)
		fsd_http_hdr_item_fini(http_hdr->args[i]);
	*/
	
	free(http_hdr);
}

int
fsd_http_hdr_write(int fd, fsd_http_status_t status, uint32_t content_len, const char *mimetype)
{
	int n;
	char buff[1024];
	
	fprintf(stderr, "HTTP HDR WRITE, CONTENT-LENGTH: %d\n", content_len);
	
	n = snprintf((char *)buff, sizeof(buff), "%s %s %s\r\nServer: %s\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n",
				 FSD_HTTP_VERSION, status.code, status.name,
				 FSD_HTTP_SERVER,
				 content_len,
				 mimetype
	); 
	
	n = write(fd, (const char *)buff, strlen(buff));
	
	if((n < 1 && errno != EINTR) || errno == EPIPE)
		return (ERR_SOCKET);
	
	return (ERR_OK);
}

int
fsd_http_hdr_set_attr(fsd_http_hdr_t *http_hdr, const char *name, const char *value)
{
	int err;
	
	if ((err = fsd_http_hdr_set(http_hdr->attrs, http_hdr->attrs_cnt, name, value)) != ERR_OK)
		return (err);
	
	http_hdr->attrs_cnt++;
	
	return (ERR_OK);
}

const char *
fsd_http_hdr_get_attr(fsd_http_hdr_t *http_hdr, const char *name)
{
	
	return (fsd_http_hdr_get(http_hdr->attrs, name));
}

int
fsd_http_hdr_set_arg(fsd_http_hdr_t *http_hdr, const char *name, const char *value)
{
	int err;
	
	if ((err = fsd_http_hdr_set(http_hdr->args, http_hdr->args_cnt, name, value)) != ERR_OK)
		return (err);
	
	http_hdr->attrs_cnt++;
	
	return (ERR_OK);
}

const char *
fsd_http_hdr_get_arg(fsd_http_hdr_t *http_hdr, const char *name)
{
	
	return (fsd_http_hdr_get(http_hdr->args, name));
}

int
fsd_http_hdr_set_args_from_url(fsd_http_hdr_t *hdr, const char *url)
{
	int err;
	unsigned int i, cnt, subcnt;
	char buff[4096], *ptr, **tab = NULL, **subtab = NULL;
	
	if (url == NULL || strlen(url) == 0)
		return (ERR_PARAM);
	
	const char *tmp = fsd_http_url_decode(url);
	ptr = tmp;
	
	//fprintf(stderr, "DECODED URL = %s", tmp);
	
	if (ptr[0] == '/')
		ptr++;
	if (ptr[0] == '?')
		ptr++;
	
	//fprintf (stderr, "LSTRIPED URL = %s", (const char *)ptr);
	
	if ((err = str_split(ptr, "&", &cnt, &tab)) != ERR_OK)
		return (err);
	
	if (cnt > FSD_HTTP_HDR_ARGS_MAX)
		return (ERR_PARAM);
	
	for (i = 0; i < cnt; i++) {
		if ((err = str_split((const char *)tab[i], "=", &subcnt, &subtab)) != ERR_OK)
			continue;
		
		if (subcnt == 2) {
			//fprintf(stderr, "ADDING HDR ARG %s=%s\n", (const char*)subtab[0], (const char*)subtab[1]);
			fsd_http_hdr_set_arg(hdr, (const char*)subtab[0], (const char*)subtab[1]);
		}
		
		//fprintf(stderr, "strs_free subtab\n");
		strs_free(subtab);
	}
	
	strs_free(tab);
	
	return (ERR_OK);
}

static void
fsd_http_sigpipe_handler(int sig)
{
	/* BROKEN PIPE/CONNECTION LOST */
	return;
}

int
fsd_http_handler(fsd_session_t *session, const char *headline)
{
	int err, opt, i;
	unsigned int len;
	char buff[1024], METHOD[32], URL[1024], HTTP[32];
	char *ptr, *tmp;
	fsd_http_hdr_t *hdr;
	
	signal(SIGPIPE, fsd_http_sigpipe_handler);
	
	fs_debug(session->log, "[ HTTP ] Spliting headline");
	
	ptr = headline;
	
	/** READING METHOD */
	if ((tmp = strstr(ptr, " ")) == NULL)
		return (ERR_PARAM);
	
	len = strlen(ptr) - strlen(tmp);
	strncpy(METHOD, ptr, len);
	METHOD[len] = '\0';
	ptr += len + 1;
	fs_debug(session->log, "[ HTTP ] METHOD = %s", METHOD);
	
	/** READING URL */
	if ((tmp = strstr(ptr, " ")) == NULL)
		return (ERR_PARAM);
	
	len = strlen(ptr) - strlen(tmp);
	strncpy(URL, ptr, len);
	URL[len] = '\0';
	ptr += len + 1;
	fs_debug(session->log, "[ HTTP ] URL = %s", URL);
	
	/** READING HTTP VERSION */
	if ((len = strlen(ptr)) == 0)
		return (ERR_PARAM);
	
	strncpy(HTTP, ptr, len);
	HTTP[len] = '\0';
	fs_debug(session->log, "[ HTTP ] HTTP = %s", HTTP);
		
	fs_debug(session->log, "[ HTTP ] Initialize HTTP header");
	
	if ((err = fsd_http_hdr_init(&hdr)) != ERR_OK)
		return (err);
		
	fs_debug(session->log, "[ HTTP ] Setting HTTP header attrs (method, url, http)");
	
	/* Ignoring http header attrs */
	while (read(session->btp->fd, 4096, buff) >= 4096) {}
	
	fsd_http_hdr_set_args_from_url(hdr, URL);
	
	if (strncmp(METHOD, "GET", strlen(METHOD)) == 0) {
		err = fsd_http_proc_req_get(session, hdr);
	} else if (strncmp(METHOD, "HEAD", strlen(METHOD)) == 0) {
		err = fsd_http_proc_req_head(session, hdr);
	} else if (strncmp(METHOD, "PUT", strlen(METHOD)) == 0) {
		err = fsd_http_proc_req_put(session, hdr);
	} else {
		err = fsd_http_error(session, FSD_HTTP_405); /* METHOD NOT ALLOWED */
	}
	
	fsd_http_hdr_fini(hdr);
	
	return (err);
}

int
fsd_http_error(fsd_session_t *session, fsd_http_status_t status)
{
	int n;
	uint32_t content_len;
	char buff[1024];
	
	n = sprintf(buff, "<html><head></head><body><h2>FSTORAGE SERVER</h2><h5>ERROR %s %s</h5></body></html>", status.code, status.name);
	content_len = strlen(buff);
	
	fsd_http_hdr_write(session->btp->fd, status, content_len, "text/html");
	
	n = write(session->btp->fd, (char*)buff, content_len);
	
	if((n < 1 && errno != EINTR) || errno == EPIPE)
		return (ERR_SOCKET);
	
	return (ERR_OK);
}

int
fsd_http_proc_req_get(fsd_session_t *session, fsd_http_hdr_t *req_hdr)
{
	FILE *fd;
	int err, rc, n, opt;
	off_t offset = 0;
	size_t filesize, partsize, bytes;
	uint16_t id_mimetype = 0;
	uint32_t content_len = 0;
	fsd_http_status_t status;
	char buff[4096], filepath[256], hdrpath[256], *hash;
	fsd_hdr_t *hdr;
	
	fs_debug(session->log, "[ HTTP ] Pprocessing REQUEST GET");

	status = FSD_HTTP_200;
	
	if ((hash = fsd_http_hdr_get_arg(req_hdr, "fkey")) == NULL)
		return (fsd_http_error(session, FSD_HTTP_400) || ERR_PARAM); /* BAD REQUEST */
	
	fs_debug(session->log, "[HTTP] hash = %s", hash);
	
	if ((err = fs_file_path_from_hash(hash, session->cfg->path, (char *)&filepath)) != ERR_OK)
		return (err);

	n = sprintf((char *)hdrpath, "%s.hdr", (const char *)filepath);
	
	/* checking if header file exist */
	if (!fs_file_exist(hdrpath)) {
		fs_debug(session->log, "Header file (%s) for Hash = %s NOT exist", hdrpath, hash);
		return (fsd_http_error(session, FSD_HTTP_404) || ERR_DOES_NOT_EXIST);
	}
	
	/* checking if content file exist */
	if (!fs_file_exist(filepath)) {
		fs_debug(session->log, "Content file (%s) for Hash = %s NOT exist '%s'", filepath, hash);
		return (fsd_http_error(session, FSD_HTTP_404) || ERR_DOES_NOT_EXIST);
	}
	
	fs_debug(session->log, "Reading header file");
	
	if ((err = fsd_hdr_read(hdrpath, &hdr)) != ERR_OK) {
		fs_debug(session->log, "Could not read header file (%s)", hdrpath);
		return (fsd_http_error(session, FSD_HTTP_500) || err); /* INTERNAL SERVER ERROR */
	}
	
	content_len = fsd_hdr_get_size(hdr);
	id_mimetype = fsd_hdr_get_mimetype(hdr);
	
	fs_debug(session->log, "SIZE Header file = %d", content_len);
	fs_debug(session->log, "MIMETYPE Header file = %s", fs_mimetype_get_name_by_id(id_mimetype));
	
	fsd_hdr_fini(hdr);
		
	if ((err = fsd_http_hdr_write(session->btp->fd, status, content_len, fs_mimetype_get_name_by_id(id_mimetype))) != ERR_OK)
		return (err);
	
	fs_debug(session->log, "Opening content file");
	fd = open(filepath, O_RDONLY);
		//return (fsd_http_error(session, FSD_HTTP_500) || ERR_FILESYSTEM); 
	
	/* Writing file to socket */
	filesize = content_len;
	bytes = 0;
	partsize = 1; //BTP_FILE_PART_SIZE;
	err = ERR_OK;
	
	fs_debug(session->log, "Streaming content file");

	//opt = 1;
	//setsockopt(session->btp->fd, IPPROTO_TCP, TCP_CORK, &opt, sizeof(opt));
	
	offset = 0;
	bytes = 0;
	while (bytes < content_len) {
		
		partsize = (partsize = 1 << 16) > (content_len - bytes) ? (content_len - bytes) : partsize;
		
		rc = fsd_sendfile(session->btp->fd, fd, &offset, partsize);
        
		if (rc == -1) {
			fs_error(session->log, "error from sendfile: %s", strerror(errno));
			break;
		} else if (rc != partsize) {
			fs_error(session->log, "incomplete transfer from sendfile: %d of %d bytes", rc, content_len);
			break;
		}
		
		bytes += partsize;
		
	} /* end of while */
	
	close(fd);
	
	return (err);
}

int
fsd_http_proc_req_head(fsd_session_t *session, fsd_http_hdr_t *head_hdr)
{
	int err, n;
	uint16_t id_mimetype = 0;
	uint32_t content_len = 0;
	fsd_http_status_t status;
	char buff[4096], filepath[256], hdrpath[256], *hash;
	fsd_hdr_t *hdr;
	
	fs_debug(session->log, "[ HTTP ] Pprocessing REQUEST HEAD");

	status = FSD_HTTP_200;
	
	if ((hash = fsd_http_hdr_get_arg(head_hdr, "fkey")) == NULL)
		return (fsd_http_error(session, FSD_HTTP_400) || ERR_PARAM); /* BAD REQUEST */
	
	fs_debug(session->log, "[HTTP] hash = %s", hash);
	
	if ((err = fs_file_path_from_hash(hash, session->cfg->path, (char *)&filepath)) != ERR_OK)
		return (err);

	n = sprintf((char *)hdrpath, "%s.hdr", (const char *)filepath);
	
	/* checking if header file exist */
	if (!fs_file_exist(hdrpath)) {
		fs_debug(session->log, "Header file (%s) for Hash = %s NOT exist", hdrpath, hash);
		return (fsd_http_error(session, FSD_HTTP_404) || ERR_DOES_NOT_EXIST);
	}
	
	/* checking if content file exist */
	if (!fs_file_exist(filepath)) {
		fs_debug(session->log, "Content file (%s) for Hash = %s NOT exist '%s'", filepath, hash);
		return (fsd_http_error(session, FSD_HTTP_404) || ERR_DOES_NOT_EXIST);
	}
	
	fs_debug(session->log, "Reading header file");
	
	if ((err = fsd_hdr_read(hdrpath, &hdr)) != ERR_OK) {
		fs_debug(session->log, "Could not read header file (%s)", hdrpath);
		return (fsd_http_error(session, FSD_HTTP_500) || err); /* INTERNAL SERVER ERROR */
	}
	
	content_len = fsd_hdr_get_size(hdr);
	id_mimetype = fsd_hdr_get_mimetype(hdr);
	
	fs_debug(session->log, "SIZE Header file = %d", content_len);
	fs_debug(session->log, "MIMETYPE Header file = %s", fs_mimetype_get_name_by_id(id_mimetype));
	
	fsd_hdr_fini(hdr);
		
	if ((err = fsd_http_hdr_write(session->btp->fd, status, content_len, fs_mimetype_get_name_by_id(id_mimetype))) != ERR_OK)
		return (err);
	
	return (ERR_OK);
}

int
fsd_http_proc_req_post(fsd_session_t *session, fsd_http_hdr_t *http_hdr)
{
}

int
fsd_http_proc_req_put(fsd_session_t *session, fsd_http_hdr_t *put_hdr)
{
	FILE *fd;
	int err, rc, n, opt;
	off_t offset = 0;
	size_t filesize, partsize, bytes;
	uint16_t id_mimetype = 0;
	uint32_t content_len = 0;
	fsd_http_status_t status;
	char buff[4096], filepath[256], hdrpath[256], *hash;
	fsd_hdr_t *hdr;
	
	fs_debug(session->log, "[ HTTP ] Pprocessing REQUEST GET");

	status = FSD_HTTP_200;
	
	if ((hash = fsd_http_hdr_get_arg(put_hdr, "hash")) == NULL)
		return (fsd_http_error(session, FSD_HTTP_400) || ERR_PARAM); /* BAD REQUEST */
	
	fs_debug(session->log, "[HTTP] hash = %s", hash);
	
	if ((err = fs_file_path_from_hash(hash, session->cfg->path, (char *)&filepath)) != ERR_OK)
		return (err);

	n = sprintf((char *)hdrpath, "%s.hdr", (const char *)filepath);
	
	/* checking if header file exist */
	if (fs_file_exist(hdrpath)) {
		fs_debug(session->log, "Header file (%s) for Hash = %s EXIST", hdrpath, hash);
		return (fsd_http_error(session, FSD_HTTP_404) || ERR_DOES_NOT_EXIST);
	}
	
	//fs_debug(session->log, "Opening content file");
	//fd = open(filepath, O_RDONLY);
		//return (fsd_http_error(session, FSD_HTTP_500) || ERR_FILESYSTEM); 
	
	/* Reading file from socket */
	n = 1;
	memset(buff, 0, 4096);
	while(fsd_http_readline(session->btp->fd, 4096, (char *)&buff) > 0) {
		fs_debug(session->log, "Read %s", buff);
		if ((n % 2) == 0 && strncmp(buff, "0", 1) == 0)
			break;
		n++;
		memset(buff, 0, 4096);
	}
	
	//close(fd);
	
	if ((err = fsd_http_hdr_write(session->btp->fd, status, content_len, fs_mimetype_get_name_by_id(id_mimetype))) != ERR_OK)
		return (err);
	
	return (err);
}

int
fsd_http_proc_req_delete(fsd_session_t *session, fsd_http_hdr_t *http_hdr)
{
}

const char *
fsd_http_url_decode(const char *url)
{
	int i, chnum;
	char buff[4096], *buffp, *urlp;
	
	if (url == NULL || strlen(url) == 0)
		return NULL;
	
	buffp = buff;
	urlp = url;
	
	i = 0;
	while (true) {
		
		if (i > 4096 || urlp[0] == '\0' || urlp[0] == NULL) { 
			buffp[0] = '\0';
			break;
		}
		
		if (urlp[0] == '%') {
			
			if (strlen(urlp) > 2) {
				char tmpstr[] = "0x0__";
				tmpstr[3] = urlp[1];
				tmpstr[4] = urlp[2];
				chnum = strtol(tmpstr, NULL, 16);
				buffp[0] = (char)chnum;
				urlp += 3;
			} else
				break;
			
		} else {
			buffp[0] = urlp[0];
			urlp += 1;
		}
		
		buffp += 1;
		i++;
	} // end of while
	
	buffp[i] = '\0';
	
	return ((const char *)buff);
}

ssize_t
fsd_http_readline(int fd, ssize_t max_len, char *linep)
{
	ssize_t n, rc;
	char c, *ptr;
	
	ptr = linep;
	
	for (n = 1; n < max_len; n++) {
		if ( (rc = read(fd, &c, 1)) == 1) {
			
			if (c != '\n' && c != '\r')
				*ptr++ = c;
			
			if (c == '\n') 
				break; /* zachowujemy znak nowej lini - podobnie jak w gets */
			
		} else if (rc == 0) {
			
			if (n == 1)
				return (0); /* znacznik konca pliku, bez pobrania danych */
			else
				break; /* znacznik konca pliku, pobrano czesc danych */
		} else 
			return (-1);
	}
	
	*ptr = 0;
	
	return (n);
}
