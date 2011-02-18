/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_mime_types.h"
#include "fstorage_str.h"

const char *
fs_mimetype_get_file_extension_from_filename(const char *filename)
{
	int err;
	unsigned int i, cnt, len;
	char buff[1024], **tab = NULL;
	
	if (filename == NULL || strlen(filename) == 0 || strlen(filename) > 1024)
		return (ERR_PARAM);
	
	if ((err = str_strip(filename, " \t\n\r", (char *)&buff)) != ERR_OK)
		return (err);
	
	if ((err = str_split(filename, ".", &cnt, &tab)) != ERR_OK)
		return (err);
	
	if (cnt > 1) {
		len = strlen(tab[cnt -1]);
		strncpy(&buff, (const char *)tab[cnt -1], len + 1);
		buff[len] = '\0';
	} else 
		*buff = '\0';
	
	strs_free(tab);
	
	return ((const char *)buff);
}

uint16_t
fs_mimetype_guess_id_from_file_extension(const char *file_ext)
{
	unsigned int i;
	
	i = 0;
	while(true) {
		if (fs_mimetypes[i].file_ext == NULL) {
			i = 0;
			break;
		} else {
			if (strcmp(fs_mimetypes[i].file_ext, file_ext) == 0)
				break;
		}
		i++;
	}
	
	return (i);
}

const char *
fs_mimetype_guess_name_from_file_extension(const char *file_ext)
{
	unsigned int i;
	
	i = 0;
	while(true) {
		if (fs_mimetypes[i].file_ext == NULL) {
			i = 0;
			break;
		} else {
			if (strcmp(fs_mimetypes[i].file_ext, file_ext) == 0)
				break;
		}
		i++;
	}
	
	return (fs_mimetypes[i].name);
}

uint16_t
fs_mimetype_get_id_from_filename(const char *filename)
{
	
	return (fs_mimetype_guess_id_from_file_extension(fs_mimetype_get_file_extension_from_filename(filename)));
}

const char *
fs_mimetype_get_name_from_filename(const char *filename)
{
	
	return (fs_mimetype_guess_name_from_file_extension(fs_mimetype_get_file_extension_from_filename(filename)));
}

const char *
fs_mimetype_get_name_by_id(uint16_t id)
{
	
	return ((fs_mimetypes+id) != NULL ? fs_mimetypes[id].name : fs_mimetypes[0].name);
}

uint16_t
fs_mimetype_get_id_by_name(const char *name)
{
	unsigned int i;
	
	i = 0;
	while(true) {
		if (fs_mimetypes[i].name == NULL) {
			i = 0;
			break;
		} else {
			if (strcmp(fs_mimetypes[i].name, name) == 0)
				break;
		}
		i++;
	}
	
	return (i);
}
