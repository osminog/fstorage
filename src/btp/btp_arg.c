/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_btp.h"
#include <stdio.h>

int
btp_arg_init(btp_arg_t **argp)
{
        btp_arg_head_t *arg_head;
	
	if ((arg_head = malloc(BTP_ARG_HEAD_SIZE)) == NULL)
		return (ERR_MEMORY);
	
	arg_head->magic = BTP_ARG_MAGIC;
	arg_head->size = BTP_ARG_HEAD_SIZE;
	arg_head->nsize = 0;
	arg_head->vsize = 0;
	arg_head->type = 0;
	arg_head->id = 0;
	
        *argp = (btp_arg_t *)arg_head;
	
        return (ERR_OK);
}

void
btp_arg_fini(btp_arg_t *arg)
{
        
        if (arg == NULL)
                return;
        
        free((btp_arg_head_t *)arg);
}

bool
btp_arg_check(btp_arg_t *arg)
{
	
	return (BTP_CHECK_ARG((btp_arg_head_t *)arg));
}

static int
btp_arg_create(uint8_t type, const char *name, int64_t number, const char *str, unsigned char *binary, uint32_t size, bool boolean, btp_arg_t **argp)
{
	int err;
	btp_value_t *value;
	btp_arg_head_t *head;
	btp_arg_t *ptr;
	uint32_t nsize, vsize;
	
	if ((err = btp_arg_init(argp)) != ERR_OK)
		return (err);
	
	switch(type) {
	case BTP_TYPE_NUMBER:
		if ((err = btp_value_create_number(number, &value)) != ERR_OK)
			return (err);
		break;
	case BTP_TYPE_STRING:
		if ((err = btp_value_create_string(str, &value)) != ERR_OK)
			return (err);
		break;
	case BTP_TYPE_BINARY:
		if ((err = btp_value_create_binary(binary, size, &value)) != ERR_OK)
			return (err);
		break;
	case BTP_TYPE_BOOL:
		if ((err = btp_value_create_bool(boolean, &value)) != ERR_OK)
			return (err);
		break;
	default:
		return (ERR_TYPE);
	}
	
	nsize = strlen(name) + 1;
	vsize = btp_value_size(value);
	
	if ((*argp = realloc((btp_arg_head_t *)(*argp), BTP_ARG_HEAD_SIZE + nsize + vsize)) == NULL)
		return (ERR_MEMORY);
	
	head = (btp_arg_head_t *)(*argp);
	head->nsize = nsize;
	head->vsize = vsize;
	head->size += nsize;
	
	strncpy((char *)head->name, name, nsize);
	
	ptr = *argp;
	ptr += head->size;
	head->size += vsize;
	
	memcpy(ptr, value, vsize);
	
	btp_value_fini(value);
	
	return (ERR_OK);
}

int
btp_arg_create_number(const char *name, int64_t number, btp_arg_t **argp)
{
	
	return btp_arg_create(BTP_TYPE_NUMBER, name, number, NULL, NULL, 0, false, argp);
}

int
btp_arg_create_string(const char *name, const char *str, btp_arg_t **argp)
{
	
	return btp_arg_create(BTP_TYPE_STRING, name, 0, str, NULL, 0, false, argp);
}

int
btp_arg_create_binary(const char *name, unsigned char *binary, uint32_t size, btp_arg_t **argp)
{
	
	return btp_arg_create(BTP_TYPE_BINARY, name, 0, NULL, binary, size, false, argp);
}

int
btp_arg_create_bool(const char *name, bool boolean, btp_arg_t **argp)
{
	
	return btp_arg_create(BTP_TYPE_BOOL, name, 0, NULL, NULL, 0, boolean, argp);
}

int
btp_arg_get_number(btp_arg_t *arg, int64_t *valuep)
{
	btp_value_t *value;
	
	value = btp_arg_value(arg);
	
	if (btp_value_type(value) != BTP_TYPE_NUMBER)
		return (ERR_TYPE);
	
	*valuep = btp_value_get_number(value);
	
	return (ERR_OK);
}

int
btp_arg_get_string(btp_arg_t *arg, char **valuep)
{
	btp_value_t *value;
	
	value = btp_arg_value(arg);
	
	if (btp_value_type(value) != BTP_TYPE_STRING)
		return (ERR_TYPE);
	
	*valuep = btp_value_get_string(value);
	
	return (ERR_OK);
}

int
btp_arg_get_binary(btp_arg_t *arg, uint32_t *size, unsigned char **valuep)
{
	btp_value_t *value;
	
	value = btp_arg_value(arg);
	
	if (btp_value_type(value) != BTP_TYPE_BINARY)
		return (ERR_TYPE);
	
	*valuep = btp_value_get_binary(value, size);
	
	return (ERR_OK);
}

int
btp_arg_get_bool(btp_arg_t *arg, bool *valuep)
{
	btp_value_t *value;
	
	value = btp_arg_value(arg);
	
	if (btp_value_type(value) != BTP_TYPE_BOOL)
		return (ERR_TYPE);
	
	*valuep = btp_value_get_bool(value);
	
	return (ERR_OK);
}

uint32_t
btp_arg_size(btp_arg_t *arg)
{
	
	return (((btp_arg_head_t *)arg)->size);
}

uint16_t
btp_arg_nsize(btp_arg_t *arg)
{
	
	return (((btp_arg_head_t *)arg)->nsize);
}


uint32_t
btp_arg_vsize(btp_arg_t *arg)
{
	
	return (((btp_arg_head_t *)arg)->vsize);
}

uint8_t
btp_arg_type(btp_arg_t *arg)
{
	
	return (btp_value_type(btp_arg_value(arg)));
}

const char *
btp_arg_name(btp_arg_t *arg)
{
	
	return ((const char *)((btp_arg_head_t *)arg)->name);
}

btp_value_t *
btp_arg_value(btp_arg_t *arg)
{
	btp_arg_t *argp = arg;
	btp_value_t *value = NULL;
	
	argp += BTP_ARG_HEAD_SIZE + btp_arg_nsize(arg);
	value = (btp_value_t *)argp;
	
	return (value);
}

