/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_impl.h"
#include "fstorage_backend.h"
#include "fstorage_conf.h"

int
fstorage_conf_init(const char *backends, unsigned int debug, const char *logfile, fs_conf_t **cfgp)
{
	int err;
	unsigned int cnt;
	fs_log_t *log;
	
	if ((err = fs_log_init(NULL, &log)) != ERR_OK)
		err = ERR_CONFIG;
	
	if (err == ERR_OK && (*cfgp = malloc(sizeof(fs_conf_t))) == NULL) {
		err = ERR_MEMORY;
	} else if (err == ERR_OK) {
		memset((*cfgp)->backends, 0, FS_BACKENDS_LIMIT);
	}
	
	if (err == ERR_OK && (err = fstorage_backends_parse(backends, &cnt, (fs_backend_t ***)(*cfgp)->backends)) != ERR_OK)
		fs_error(log, "[cfg] Error while parsing backends. Check it is properly configured");
	
	
	/* Checking for logfile */
	if (err == ERR_OK && logfile != NULL && strlen(logfile) > 0) {
		strncpy((char *)&(*cfgp)->logfile, logfile, strlen(logfile) + 1);
		(*cfgp)->logfile[strlen(logfile)] = '\0';
	} else if (err == ERR_OK) {
		(*cfgp)->logfile[0] = '\0';
	}
	
	if (err == ERR_OK) {
		(*cfgp)->debug = debug;
		(*cfgp)->backends_cnt = cnt;
	} else {
		fstorage_conf_fini(*cfgp);
	}

	fs_log_fini(log);
	
	return (err);
}

void
fstorage_conf_fini(fs_conf_t *cfg)
{
	
	if (cfg == NULL)
		return;
	
	fstorage_backends_fini(cfg->backends);
	
	free(cfg);
}
