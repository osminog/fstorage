/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Katjowy Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "fstorage_errcode.h"
#include "fstoraged_hdr.h"

int
fsd_hdr_file_create(uint8_t type, uint16_t parts, uint16_t mimetype, uint32_t file_size, uint32_t flags, 
		      uint32_t expire_time, fsd_hdr_t **hdr)
{
	
	if ((*hdr = malloc(sizeof(fsd_hdr_t))) == NULL)
		return (ERR_MEMORY);
	
	(*hdr)->type = type;
	(*hdr)->parts = parts;
	(*hdr)->mimetype = mimetype;
	(*hdr)->size = file_size;
	(*hdr)->flags = flags;
	(*hdr)->expire_tm = expire_time;
	
	return (ERR_OK);
}

int
fsd_hdr_cache_create(uint8_t type, uint16_t parts, uint16_t mimetype, uint32_t cache_size, uint32_t flags, 
		      uint32_t expire_time, unsigned char* data, uint32_t data_size, fsd_hdr_t **hdr)
{
	unsigned char *ptr;
	
	if ((*hdr = malloc(sizeof(fsd_hdr_t) + data_size)) == NULL)
		return (ERR_MEMORY);
	
	(*hdr)->type = type;
	(*hdr)->parts = parts;
	(*hdr)->mimetype = mimetype;
	(*hdr)->size = cache_size;
	(*hdr)->flags = flags;
	(*hdr)->expire_tm = expire_time;
	
	if (data != NULL && data_size > 0) {
		ptr = (unsigned char *)(*hdr);
		ptr += sizeof(fsd_hdr_t);
		memcpy(ptr, data, data_size);
	}
	
	return (ERR_OK);
}
		      
int
fsd_hdr_fini(fsd_hdr_t *hdr)
{
	
	if (hdr == NULL)
		return;
	
	free(hdr);
}

void
fsd_hdr_set_type(fsd_hdr_t *hdr, uint8_t type)
{
	
	(hdr)->type = type;
}

uint8_t
fsd_hdr_get_type(fsd_hdr_t *hdr)
{
	
	return ((hdr)->type);
}

void
fsd_hdr_set_parts(fsd_hdr_t *hdr, uint16_t parts)
{
	
	(hdr)->parts = parts;
}

uint16_t
fsd_hdr_get_parts(fsd_hdr_t *hdr)
{
	
	return ((hdr)->parts);
}

void
fsd_hdr_set_size(fsd_hdr_t *hdr, uint32_t size)
{
	
	(hdr)->size = size;
}

uint32_t
fsd_hdr_get_size(fsd_hdr_t *hdr)
{
	
	return ((hdr)->size);
}

void
fsd_hdr_set_create_time(fsd_hdr_t *hdr, uint32_t create_time)
{
	
	(hdr)->create_tm = create_time;
}

uint32_t
fsd_hdr_get_create_time(fsd_hdr_t *hdr)
{
	
	return ((hdr)->create_tm);
}

void
fsd_hdr_set_expire_time(fsd_hdr_t *hdr, uint32_t expire_time)
{
	
	(hdr)->expire_tm = (expire_time > 0) ? (time(NULL)) + expire_time : 0;
}

uint32_t
fsd_hdr_get_expire_time(fsd_hdr_t *hdr)
{
	
	return ((hdr)->expire_tm);
}

void fsd_hdr_set_mimetype(fsd_hdr_t *hdr, uint16_t mimetype)
{
	
	(hdr)->mimetype = mimetype;
}

uint32_t
fsd_hdr_get_mimetype(fsd_hdr_t *hdr)
{
	
	return ((hdr)->mimetype);
}

void
fsd_hdr_set_flags(fsd_hdr_t *hdr, uint32_t flags)
{
	
	hdr->flags |= flags;
}

void
fsd_hdr_unset_flags(fsd_hdr_t *hdr, uint32_t flags)
{
	
	hdr->flags &= ~flags;
}

uint32_t
fsd_hdr_get_flags(fsd_hdr_t *hdr)
{
	
	return (hdr->flags);
}

bool
fsd_hdr_has_flags(fsd_hdr_t *hdr, uint32_t flags)
{
	
	return ((hdr->flags & flags) > 0);
}

int
fsd_hdr_read(const char *filepath, fsd_hdr_t **hdr)
{
	int n;
	FILE *fd;
	unsigned char *ptr;
	
	if (!(fd = fopen(filepath, "rb")))
		return (ERR_FILESYSTEM);
	
	if ((ptr = malloc(sizeof(fsd_hdr_t))) == NULL)
		return (ERR_MEMORY);
	
	if (fread(ptr, 1, sizeof(fsd_hdr_t), fd) != sizeof(fsd_hdr_t)) {
		free(ptr);
		return (ERR_FILESYSTEM);
	}
		
	*hdr = (fsd_hdr_t *)ptr;
	
	return (ERR_OK);
}

int
fsd_hdr_write(fsd_hdr_t *hdr, const char *filepath)
{
	FILE *fd;
	unsigned char *ptr;
	
	if (!(fd = fopen(filepath, "wb")))
		return (ERR_FILESYSTEM);
	
	ptr = (unsigned char *)hdr;
	
	if (fwrite(ptr, 1, sizeof(fsd_hdr_t), fd) != sizeof(fsd_hdr_t))
		return (ERR_FILESYSTEM);
	
	fflush(fd);
	fclose(fd);
	
	return (ERR_OK);
}
