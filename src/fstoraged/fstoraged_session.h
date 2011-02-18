/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGED_SESSION_H_
#define	_FSTORAGED_SESSION_H_

#include "fstorage_btp.h"
#include "fstorage_log.h"
#include "fstoraged_conf.h"

/* SESSION TYPES */
typedef struct s_fsd_session {
	int state;
	uint64_t id;
	btp_t *btp;
	fs_log_t *log;
	fsd_conf_t *cfg;
	char ipaddr[32];
} fsd_session_t;

int fsd_session_init(fsd_conf_t *cfg, fs_log_t *log, int sock, fsd_session_t **sessionp);
void fsd_session_fini(fsd_session_t *session);

void fsd_session_set_state(fsd_session_t *session, int state);
int fsd_session_get_state(fsd_session_t *session);

void fsd_session_set_ipaddr(fsd_session_t *session, const char *ipaddr);
const char *fsd_session_get_ipaddr(fsd_session_t *session);

#endif	/* !_FSTORAGED_SESSION_H_ */
