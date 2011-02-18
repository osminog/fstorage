/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGED_CLIENT_H_
#define	_FSTORAGED_CLIENT_H_

#include <pthread.h>
#include <stdint.h>

#include "fstoraged_conf.h"

int fsd_client(fsd_conf_t *cfg, fs_log_t *log, int sock, const char *ipaddr);
void *fsd_client_thread(void *data);

#endif	/* !_FSTORAGED_CLIENT_H_ */
