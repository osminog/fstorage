/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGED_FSTP_H_
#define	_FSTORAGED_FSTP_H_

#include "fstoraged_session.h"

int fsd_fstp_handler(fsd_session_t *session, const char *headline);

int fsd_fstp_snd(fsd_session_t *session, btp_pkg_t *pkg);
int fsd_fstp_rcv(fsd_session_t *session, btp_pkg_t **pkgp);

int fsd_fstp_none(fsd_session_t *session, btp_pkg_t *pkg_in);
int fsd_fstp_handshake(fsd_session_t *session);

int fsd_fstp_file_send(fsd_session_t *session, btp_pkg_t *pkg_in);
int fsd_fstp_file_recv(fsd_session_t *session, btp_pkg_t *pkg_in);
int fsd_fstp_file_replace(fsd_session_t *session, btp_pkg_t *pkg_in);
int fsd_fstp_file_remove(fsd_session_t *session, btp_pkg_t *pkg_in);

#endif	/* !_FSTORAGED_FSTP_H_ */
