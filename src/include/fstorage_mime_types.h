/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGE_MIME_TYPES_H_
#define	_FSTORAGE_MIME_TYPES_H_

#include <stdio.h>
#include <stdint.h>

typedef struct s_fs_mimetype {
	const char *name;
	const char *file_ext;
} fs_mimetype_t;

static fs_mimetype_t fs_mimetypes[] = {
	{"application/octet-stream", ""},
	{"text/html", "html"},
	{"text/html", "htm"},
	{"text/html", "shtml"},
	{"text/css", "css"},
	{"text/xml", "xml"},
	{"text/xml", "rss"},
	{"image/gif", "gif"},
	{"image/gif", "gif"},
	{"image/jpeg", "jpeg"},
	{"image/jpeg", "jpg"},
	{"application/x-javascript", "js"},
	{"application/atom+xml", "atom"},
	{"text/mathml", "mml"},
	{"text/plain", "txt"},
	{"text/vnd.sun.j2me.app-descriptor", "jad"},
	{"text/vnd.wap.wml", "wml"},
	{"text/x-component", "htc"},
	{"image/png", "png"},
	{"image/tiff", "tif"},
	{"image/tiff", "tiff"},
	{"image/vnd.wap.wbmp", "wbmp"},
	{"image/x-icon", "ico"},
	{"image/x-jng", "jng"},
	{"image/x-ms-bmp", "bmp"},
	{"application/java-archive", "jar"},
	{"application/java-archive", "war"},
	{"application/java-archive", "ear"},
	{"application/mac-binhex40", "hqx"},
	{"application/msword", "doc"},
	{"application/pdf", "pdf"},
	{"application/postscript", "ps"},
	{"application/postscript", "eps"},
	{"application/postscript", "ai"},
	{"application/rtf", "rtf"},
	{"application/vnd.ms-excel", "xls"},
	{"application/vnd.ms-powerpoint", "ppt"},
	{"application/vnd.wap.wmlc", "wmlc"},
	{"application/vnd.wap.xhtml+xml", "xhtml"},
	{"application/x-cocoa", "cco"},
	{"application/x-java-archive-diff", "jardiff"},
	{"application/x-java-jnlp-file", "jnlp"},
	{"application/x-makeself", "run"},
	{"application/x-perl", "pl"},
	{"application/x-perl", "pm"},
	{"application/x-pilot", "prc"},
	{"application/x-pilot", "pdb"},
	{"application/x-rar-compressed", "rar"},
	{"application/x-redhat-package-manager", "rpm"},
	{"application/x-sea", "sea"},
	{"application/x-shockwave-flash", "swf"},
	{"application/x-stuffit", "sit"},
	{"application/x-tcl", "tcl"},
	{"application/x-tcl", "tk"},
	{"application/x-x509-ca-cert", "der"},
	{"application/x-x509-ca-cert", "pem"},
	{"application/x-x509-ca-cert", "crt"},
	{"application/x-xpinstall", "xpi"},
	{"application/zip", "zip"},
	{"application/octet-stream", "dll"},
	{"application/octet-stream", "bin"},
	{"application/octet-stream", "exe"},
	{"application/octet-stream", "deb"},
	{"application/octet-stream", "dmg"},
	{"application/octet-stream", "eot"},
	{"application/octet-stream", "iso"},
	{"application/octet-stream", "img"},
	{"application/octet-stream", "msi"},
	{"application/octet-stream", "msp"},
	{"application/octet-stream", "msm"},
	{"audio/midi", "mid"},
	{"audio/midi", "midi"},
	{"audio/midi", "kar"},
	{"audio/mpeg", "mp3"},
	{"audio/x-realaudio", "ra"},
	{"video/3gpp", "3gpp"},
	{"video/3gpp", "3gp"},
	{"video/mpeg", "mpeg"},
	{"video/mpeg", "mpg"},
	{"video/quicktime", "mov"},
	{"video/x-flv", "flv"},
	{"video/x-mng", "mng"},
	{"video/x-ms-asf", "asx"},
	{"video/x-ms-asf", "asf"},
	{"video/x-ms-wmv", "wmv"},
	{"video/x-msvideo", "avi"},
	{NULL, NULL}
};

const char *fs_mimetype_get_file_extension_from_filename(const char *filename);

uint16_t fs_mimetype_guess_id_from_file_extension(const char *file_ext);
const char *fs_mimetype_guess_name_from_file_extension(const char *file_ext);

uint16_t fs_mimetype_get_id_from_filename(const char *filename);
const char *fs_mimetype_get_name_from_filename(const char *filename);

const char *fs_mimetype_get_name_by_id(uint16_t id);
uint16_t fs_mimetype_get_id_by_name(const char *name);

#endif	/* !_FSTORAGE_MIME_TYPES_H_ */
