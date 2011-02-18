/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstoraged_session.h"

int
fsd_session_init(fsd_conf_t *cfg, fs_log_t *log, int sock, fsd_session_t **sessionp)
{
	int err;
	btp_t *btp;
	
	if ((err = btp_init(sock, &btp)) != ERR_OK)
		return (err);
	
	if ((*sessionp = malloc(sizeof(fsd_session_t))) == NULL) {
		btp_fini(btp);
		return (ERR_MEMORY);
	}
	
	(*sessionp)->btp = btp;
	(*sessionp)->log = log;
	(*sessionp)->cfg = cfg;
	
	return (ERR_OK);
}

void
fsd_session_fini(fsd_session_t *session)
{
	
	if (session == NULL)
		return;
	
	btp_fini(session->btp);
	free(session);
}

void
fsd_session_set_state(fsd_session_t *session, int state)
{
	
	session->state = state;
}

int
fsd_session_get_state(fsd_session_t *session)
{
	
	return (session->state);
}

void
fsd_session_set_ipaddr(fsd_session_t *session, const char *ipaddr)
{

	strncpy(session->ipaddr, ipaddr, strlen(ipaddr) + 1);
}

const char *
fsd_session_get_ipaddr(fsd_session_t *session)
{
	
	return (const char *)(session->ipaddr);
}
