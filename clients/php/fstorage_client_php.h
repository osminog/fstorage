/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGE_CLIENT_PHP_H_
#define	_FSTORAGE_CLIENT_PHP_H_

#include <fstorage++.h>

#define	PHP_FSTORAGE_EXT_NAME		"fstorage"
#define	PHP_FSTORAGE_EXT_VERSION	"0.5"

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
#include "zend_exceptions.h"
}

extern zend_module_entry fstorage_module_entry;
#define phpext_fstorage_ptr &fstorage_module_entry;

#endif /* !_FSTORAGE_CLIENT_PHP_H_ */
