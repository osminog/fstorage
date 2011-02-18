/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGED_HDR_H_
#define	_FSTORAGED_HDR_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define	FSD_TYPE_FILE			1
#define	FSD_TYPE_CACHE_NUMBER		2
#define	FSD_TYPE_CACHE_STRING		3
#define	FSD_TYPE_CACHE_BINARY		4
#define	FSD_TYPE_CACHE_BOOL		5

typedef struct s_fsd_hdr {
	uint8_t type;
	uint16_t parts;
	uint16_t mimetype;
	uint32_t flags;
	uint32_t size;
	uint32_t create_tm; /* Unix timestamp */
	uint32_t expire_tm; /* Unix timestamp */
} fsd_hdr_t;


int fsd_hdr_file_create(uint8_t type, uint16_t parts, uint16_t mimetype, uint32_t file_size, uint32_t flags, 
		      uint32_t expire_time, fsd_hdr_t **hdr);

int fsd_hdr_cache_create(uint8_t type, uint16_t parts, uint16_t mimetype, uint32_t cache_size, uint32_t flags, 
		      uint32_t expire_time, unsigned char* data, uint32_t data_size, fsd_hdr_t **hdr);
		      
int fsd_hdr_fini(fsd_hdr_t *hdr);

void fsd_hdr_set_type(fsd_hdr_t *hdr, uint8_t type);
uint8_t fsd_hdr_get_type(fsd_hdr_t *hdr);

void fsd_hdr_set_parts(fsd_hdr_t *hdr, uint16_t parts);
uint16_t fsd_hdr_get_parts(fsd_hdr_t *hdr);

void fsd_hdr_set_size(fsd_hdr_t *hdr, uint32_t size);
uint32_t fsd_hdr_get_size(fsd_hdr_t *hdr);

void fsd_hdr_set_create_time(fsd_hdr_t *hdr, uint32_t create_time);
uint32_t fsd_hdr_get_create_time(fsd_hdr_t *hdr);

void fsd_hdr_set_expire_time(fsd_hdr_t *hdr, uint32_t expire_time);
uint32_t fsd_hdr_get_expire_time(fsd_hdr_t *hdr);

void fsd_hdr_set_mimetype(fsd_hdr_t *hdr, uint16_t mimetype);
uint32_t fsd_hdr_get_mimetype(fsd_hdr_t *hdr);

void fsd_hdr_set_flags(fsd_hdr_t *hdr, uint32_t flags);
void fsd_hdr_unset_flags(fsd_hdr_t *hdr, uint32_t flags);
uint32_t fsd_hdr_get_flags(fsd_hdr_t *hdr);
bool fsd_hdr_has_flags(fsd_hdr_t *hdr, uint32_t flags);

int fsd_hdr_read(const char *filename, fsd_hdr_t **hdr);
int fsd_hdr_write(fsd_hdr_t *hdr, const char *filename);

#endif	/* !_FSTORAGED_HDR_H_ */
