/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGED_CONF_H_
#define	_FSTORAGED_CONF_H_

#include "fstorage_cfg.h"
#include "fstorage_log.h"

typedef struct s_fsd_conf {
	uint16_t port;
	char host[128];
	char path[256];
	char logfile[256];
	char **allowed_ips;
	unsigned int allowed_ips_cnt;
	uint64_t salt;
	uint8_t base;
	int debug;
} fsd_conf_t;

int fsd_conf_init(const char *filepath, fsd_conf_t **cfgp);
void fsd_conf_fini(fsd_conf_t *cfg);

#endif	/* !_FSTORAGED_CONF_H_ */
