/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGED_HTTP_H_
#define	_FSTORAGED_HTTP_H_

#include "fstoraged.h"
#include "fstoraged_session.h"

#define	FSD_HTTP_VERSION	"HTTP/1.1"
#define	FSD_HTTP_SERVER		"Fstorage v." FSD_VERSION

#define	FSD_HTTP_METHOD_GET	"GET"
#define	FSD_HTTP_METHOD_POST	"POST"
#define	FSD_HTTP_METHOD_HEAD	"HEAD"
#define	FSD_HTTP_METHOD_PUT	"PUT"
#define	FSD_HTTP_METHOD_DELETE	"DELETE"
#define	FSD_HTTP_METHOD_CONNECT	"CONNECT"

#define	FSD_HTTP_HDR_ARGS_MAX			32
#define	FSD_HTTP_HDR_ATTRS_MAX			32
#define	FSD_HTTP_HDR_ITEM_NAME_SIZE_MAX		64
#define	FSD_HTTP_HDR_ITEM_VALUE_SIZE_MAX	512

typedef struct s_fsd_http_status {
	const char *code;
	const char *name;
} fsd_http_status_t;

static fsd_http_status_t FSD_HTTP_100 = {"100", "Continue"};
static fsd_http_status_t FSD_HTTP_101 = {"101", "Switching Protocols"};
static fsd_http_status_t FSD_HTTP_200 = {"200", "OK"};
static fsd_http_status_t FSD_HTTP_201 = {"201", "Created"};
static fsd_http_status_t FSD_HTTP_202 = {"202", "Accepted"};
static fsd_http_status_t FSD_HTTP_203 = {"203", "Non-Authoritative Information"};
static fsd_http_status_t FSD_HTTP_204 = {"204", "No Content"};
static fsd_http_status_t FSD_HTTP_205 = {"205", "Reset Content"};
static fsd_http_status_t FSD_HTTP_206 = {"206", "Partial Content"};
static fsd_http_status_t FSD_HTTP_300 = {"300", "Multiple Choices"};
static fsd_http_status_t FSD_HTTP_301 = {"301", "Moved Permanently"};
static fsd_http_status_t FSD_HTTP_302 = {"302", "Found"};
static fsd_http_status_t FSD_HTTP_303 = {"303", "See Other"};
static fsd_http_status_t FSD_HTTP_304 = {"304", "Not Modified"};
static fsd_http_status_t FSD_HTTP_305 = {"305", "Use Proxy"};
static fsd_http_status_t FSD_HTTP_307 = {"307", "Temporary Redirect"};
static fsd_http_status_t FSD_HTTP_400 = {"400", "Bad Request"};
static fsd_http_status_t FSD_HTTP_401 = {"401", "Unauthorized"};
static fsd_http_status_t FSD_HTTP_402 = {"402", "Payment Required"};
static fsd_http_status_t FSD_HTTP_403 = {"403", "Forbidden"};
static fsd_http_status_t FSD_HTTP_404 = {"404", "Not Found"};
static fsd_http_status_t FSD_HTTP_405 = {"405", "Method Not Allowed"};
static fsd_http_status_t FSD_HTTP_406 = {"406", "Not Acceptable"};
static fsd_http_status_t FSD_HTTP_407 = {"407", "Proxy Authentication Required"};
static fsd_http_status_t FSD_HTTP_408 = {"408", "Request Time-out"};
static fsd_http_status_t FSD_HTTP_409 = {"409", "Conflict"};
static fsd_http_status_t FSD_HTTP_410 = {"410", "Gone"};
static fsd_http_status_t FSD_HTTP_411 = {"411", "Length Required"};
static fsd_http_status_t FSD_HTTP_412 = {"12", "Precondition Failed"};
static fsd_http_status_t FSD_HTTP_413 = {"413", "Request Entity Too Large"};
static fsd_http_status_t FSD_HTTP_414 = {"414", "Request-URI Too Large"};
static fsd_http_status_t FSD_HTTP_415 = {"415", "Unsupported Media Type"};
static fsd_http_status_t FSD_HTTP_416 = {"416", "Requested range not satisfiable"};
static fsd_http_status_t FSD_HTTP_417 = {"417", "Expectation Failed"};
static fsd_http_status_t FSD_HTTP_500 = {"500", "Internal Server Error"};
static fsd_http_status_t FSD_HTTP_501 = {"501", "Not Implemented"};
static fsd_http_status_t FSD_HTTP_502 = {"502", "Bad Gateway"};
static fsd_http_status_t FSD_HTTP_503 = {"503", "Service Unavailable"};
static fsd_http_status_t FSD_HTTP_504 = {"504", "Gateway Time-out"};
static fsd_http_status_t FSD_HTTP_505 = {"505", "HTTP Version not supported"};

typedef struct s_fsd_http_hdr_item {
	char name[FSD_HTTP_HDR_ITEM_NAME_SIZE_MAX + 1];
	char value[FSD_HTTP_HDR_ITEM_VALUE_SIZE_MAX + 1];
} fsd_http_hdr_item_t;

typedef struct s_fsd_http_hdr {
	uint16_t attrs_cnt;
	uint16_t args_cnt;
	fsd_http_hdr_item_t *attrs[FSD_HTTP_HDR_ATTRS_MAX];
	fsd_http_hdr_item_t *args[FSD_HTTP_HDR_ARGS_MAX];
} fsd_http_hdr_t;


int fsd_http_hdr_init(fsd_http_hdr_t **http_hdrp);
void fsd_http_hdr_fini(fsd_http_hdr_t *http_hdr);
int fsd_http_hdr_write(int fd, fsd_http_status_t status, uint32_t content_len, const char *mimetype);

int fsd_http_hdr_set_attr(fsd_http_hdr_t *http_hdr, const char *name, const char *value);
const char *fsd_http_hdr_get_attr(fsd_http_hdr_t *http_hdr, const char *name);

int fsd_http_hdr_set_arg(fsd_http_hdr_t *http_hdr, const char *name, const char *value);
const char *fsd_http_hdr_get_arg(fsd_http_hdr_t *http_hdr, const char *name);
int fsd_http_hdr_set_args_from_url(fsd_http_hdr_t *hdr, const char *url);

int fsd_http_handler(fsd_session_t *session, const char *headline);

int fsd_http_error(fsd_session_t *session, fsd_http_status_t status);
int fsd_http_proc_req_get(fsd_session_t *session, fsd_http_hdr_t *http_hdr);
int fsd_http_proc_req_head(fsd_session_t *session, fsd_http_hdr_t *http_hdr);
int fsd_http_proc_req_post(fsd_session_t *session, fsd_http_hdr_t *http_hdr);
int fsd_http_proc_req_put(fsd_session_t *session, fsd_http_hdr_t *http_hdr);
int fsd_http_proc_req_delete(fsd_session_t *session, fsd_http_hdr_t *http_hdr);

const char *fsd_http_url_decode(const char *url);
ssize_t fsd_http_readline(int fd, ssize_t max_len, char *linep);

#endif	/* !_FSTORAGED_HTTP_H_ */
