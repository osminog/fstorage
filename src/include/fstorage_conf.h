/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGE_CONF_H_
#define	_FSTORAGE_CONF_H_

#include "fstorage_impl.h"

int fstorage_conf_init(const char *backends, unsigned int debug, const char *logfile, fs_conf_t **cfgp);
void fstorage_conf_fini(fs_conf_t *cfg);

#endif	/* !_FSTORAGE_CONF_H_ */
