/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_btp.h"


int
btp_pkg_init(btp_pkg_t **pkgp)
{
	btp_pkg_head_t *pkg_head;
	
	if ((pkg_head = malloc(BTP_PKG_HEAD_SIZE)) == NULL)
		return (ERR_MEMORY);
	
	pkg_head->magic = BTP_PKG_MAGIC;
	pkg_head->opcode = 0;
	pkg_head->errcode = 0;
	pkg_head->argno = 0;
	pkg_head->flags = 0;
	pkg_head->size = BTP_PKG_HEAD_SIZE;
	
	*pkgp = (btp_pkg_t *)pkg_head;
	
	return (ERR_OK);
}

void
btp_pkg_fini(btp_pkg_t *pkg)
{
        
        if (pkg == NULL)
                return;
        
        free((btp_pkg_head_t *)pkg);
}

bool
btp_pkg_check(btp_pkg_t *pkg)
{
	
	return (BTP_CHECK_PKG((btp_pkg_head_t*)pkg));
}

void
btp_pkg_set_error(btp_pkg_t *pkg, uint32_t errcode)
{
        btp_pkg_head_t *pkg_head = (btp_pkg_head_t *)pkg;
        
        pkg_head->errcode = errcode;
}

uint32_t
btp_pkg_get_error(btp_pkg_t *pkg)
{
        btp_pkg_head_t *pkg_head = (btp_pkg_head_t *)pkg;
        
        return (pkg_head->errcode);
}

void
btp_pkg_set_opcode(btp_pkg_t *pkg, uint32_t opcode)
{
        btp_pkg_head_t *pkg_head = (btp_pkg_head_t *)pkg;
        
        pkg_head->opcode = opcode;
}

uint32_t
btp_pkg_get_opcode(btp_pkg_t *pkg)
{
        btp_pkg_head_t *pkg_head = (btp_pkg_head_t *)pkg;
        
        return (pkg_head->opcode);
}

uint32_t
btp_pkg_argno(btp_pkg_t *pkg)
{
        btp_pkg_head_t *pkg_head = (btp_pkg_head_t *)pkg;
        
        return (pkg_head->argno);
}

uint64_t
btp_pkg_size(btp_pkg_t *pkg)
{
        btp_pkg_head_t *pkg_head = (btp_pkg_head_t *)pkg;
        
        return (pkg_head->size);
}

void
btp_pkg_set_flag(btp_pkg_t *pkg, uint64_t flag)
{
        btp_pkg_head_t *pkg_head = (btp_pkg_head_t *)pkg;
        
        pkg_head->flags |= flag;
}

void
btp_pkg_unset_flag(btp_pkg_t *pkg, uint64_t flag)
{
        btp_pkg_head_t *pkg_head = (btp_pkg_head_t *)pkg;
        
        pkg_head->flags &= ~flag;
}

bool
btp_pkg_has_flag(btp_pkg_t *pkg, uint64_t flag)
{
        btp_pkg_head_t *pkg_head = (btp_pkg_head_t *)pkg;
        
        return ((pkg_head->flags & flag) > 0);
}

int
btp_pkg_add(btp_arg_t *arg, btp_pkg_t **pkgp)
{
	btp_pkg_head_t *head;
	btp_pkg_t *ptr;
	uint32_t asize, psize;
	
	asize = btp_arg_size(arg);
	psize = btp_pkg_size(*pkgp);
	
	if ((*pkgp = realloc(*pkgp, psize + asize)) == NULL)
		return (ERR_MEMORY);
	
	head = (btp_pkg_head_t *)(*pkgp);
	ptr = *pkgp;
	ptr += psize;
	
	memcpy(ptr, arg, asize);
	
	head->size += asize;
	head->argno += 1;
	
	return (ERR_OK);
}

int
btp_pkg_add_number(const char *name, int64_t number, btp_pkg_t **pkgp) 
{
	int err;
	btp_arg_t *arg;
	
	if ((err = btp_arg_create_number(name, number, &arg)) != ERR_OK)
		return (err);
	
	err = btp_pkg_add(arg, pkgp);
	
	btp_arg_fini(arg);
	
	return (err);
}

int
btp_pkg_add_string(const char *name, const char *str, btp_pkg_t **pkgp)
{
	int err;
	btp_arg_t *arg;
	
	if ((err = btp_arg_create_string(name, str, &arg)) != ERR_OK)
		return (err);
	
	err = btp_pkg_add(arg, pkgp);
	
	btp_arg_fini(arg);
	
	return (err);
}

int
btp_pkg_add_binary(const char *name, unsigned char *binary, uint32_t size, btp_pkg_t **pkgp)
{
	int err;
	btp_arg_t *arg;
	
	if ((err = btp_arg_create_binary(name, binary, size, &arg)) != ERR_OK)
		return (err);
	
	err = btp_pkg_add(arg, pkgp);
	
	btp_arg_fini(arg);
	
	return (err);
}

int
btp_pkg_add_bool(const char *name, bool boolean, btp_pkg_t **pkgp)
{
	int err;
	btp_arg_t *arg;
	
	if ((err = btp_arg_create_bool(name, boolean, &arg)) != ERR_OK)
		return (err);
	
	err = btp_pkg_add(arg, pkgp);
	
	btp_arg_fini(arg);
	
	return (err);
}


int
btp_pkg_arg_by_name(btp_pkg_t *pkg, const char *name, btp_arg_t **argp)
{
	btp_arg_t *arg;
	unsigned int cnt;
	
	BTP_FOREACH(cnt, arg, pkg) {
		if (strcmp(name, btp_arg_name(arg)) == 0) {
			*argp = arg;
			return (ERR_OK);
		}
	}
	
	return (ERR_NO_RESULT);
}

btp_arg_t *
btp_pkg_foreach_arg_first(btp_pkg_t *pkg)
{
	btp_arg_t *a = (pkg + BTP_PKG_HEAD_SIZE);
	
	return (btp_arg_check(a) ? a : NULL);
}

btp_arg_t *
btp_pkg_foreach_arg_next(btp_arg_t *arg)
{
	btp_arg_t *a = (arg + btp_arg_size(arg));
	
	return (btp_arg_check(a) ? a : NULL);
}

