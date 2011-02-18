/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_btp.h"


int
btp_value_init(btp_value_t **valuep)
{
        btp_value_head_t *value_head;
	
	if ((value_head = malloc(BTP_VALUE_HEAD_SIZE)) == NULL)
		return (ERR_MEMORY);
	
	value_head->magic = BTP_VALUE_MAGIC;
	value_head->size = BTP_VALUE_HEAD_SIZE;
	
        *valuep = (btp_value_t *)value_head;
	
        return (ERR_OK);
}

void
btp_value_fini(btp_value_t *value)
{
        
        if (value == NULL)
                return;
        
        free((btp_value_head_t *)value);
}

uint8_t
btp_value_type(btp_value_t *value)
{
	
	return (((btp_value_head_t *)value)->type);
}

uint32_t
btp_value_size(btp_value_t *value)
{
	
	return (((btp_value_head_t *)value)->size);
}

int
btp_value_create_number(int64_t number, btp_value_t **valuep)
{
	int err;
	btp_value_head_t *head;
	
	if ((err = btp_value_init(valuep)) != ERR_OK)
		return (err);
	
	head = (btp_value_head_t *)(*valuep);
	head->type = BTP_TYPE_NUMBER;
	head->number = number;
	
	return (ERR_OK);
}

int
btp_value_create_string(const char *string, btp_value_t **valuep)
{
	int err;
	unsigned int size;
	btp_value_head_t *head;
	btp_value_t *ptr;
	
	if ((err = btp_value_init(valuep)) != ERR_OK)
		return (err);
	
	size = strlen(string) + 1;
	*valuep = realloc(*valuep, BTP_VALUE_HEAD_SIZE + size);
	
	head = (btp_value_head_t *)(*valuep);
	head->type = BTP_TYPE_STRING;
	head->size += size;
	
	ptr = *valuep;
	ptr += BTP_VALUE_HEAD_SIZE;
	
	strncpy((char *)ptr, string, size);
	
	return (ERR_OK);
}

int
btp_value_create_binary(unsigned char *binary, uint32_t size, btp_value_t **valuep)
{
	int err;
	btp_value_head_t *head;
	btp_value_t *ptr;
	
	if ((err = btp_value_init(valuep)) != ERR_OK)
		return (err);
	
	*valuep = realloc(*valuep, BTP_VALUE_HEAD_SIZE + size);
	
	head = (btp_value_head_t *)(*valuep);
	head->type = BTP_TYPE_BINARY;
	head->size += size;
	
	ptr = *valuep;
	ptr += BTP_VALUE_HEAD_SIZE;
	
	memcpy(ptr, binary, size);
	
	return (ERR_OK);
}

int
btp_value_create_bool(bool boolean, btp_value_t **valuep)
{
	int err;
	btp_value_head_t *head;
	
	if ((err = btp_value_init(valuep)) != ERR_OK)
		return (err);
	
	head = (btp_value_head_t *)(*valuep);
	head->type = BTP_TYPE_BOOL;
	head->boolean = boolean;
	
	return (ERR_OK);
}

int64_t
btp_value_get_number(btp_value_t *value)
{
	
	return (((btp_value_head_t *)value)->number);
}

char *
btp_value_get_string(btp_value_t *value)
{
	btp_value_t *ptr = value;
	
	return ((char *)(ptr + BTP_VALUE_HEAD_SIZE));
}

unsigned char *
btp_value_get_binary(btp_value_t *value, uint32_t *size)
{
	btp_value_t *ptr = value;
	
	*size = btp_value_size(value) - BTP_VALUE_HEAD_SIZE;
	
	return ((unsigned char *)(ptr + BTP_VALUE_HEAD_SIZE));
}

bool
btp_value_get_bool(btp_value_t *value)
{
	
	return (((btp_value_head_t *)value)->boolean);
}

