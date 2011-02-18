/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Katjowy Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstoraged_session.h"
#include "fstoraged_fstp.h"
#include "fstoraged_http.h"
#include "fstoraged_client.h"

int
fsd_client(fsd_conf_t *cfg, fs_log_t *log, int sock, const char *ipaddr)
{
	int err, rc, opt;
	fsd_session_t *session;
	pthread_t tid;

	opt = 0;
	//setsockopt(sock, SOL_SOCKET, TCP_CORK, &opt, sizeof(opt));
	//setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	
	if ((err = fsd_session_init(cfg, log, sock, &session)) == ERR_OK) {
		session->id = 0;
		fsd_session_set_ipaddr(session, ipaddr);
		rc = pthread_create(&tid, NULL, fsd_client_thread, (void *)session);
		if (rc)
			fs_error(log, "pthread_create failed (err=%d)", rc);
	} else
		fs_error(log, "Cannot initialize client session (err=%d)", err);
	
	return (err);
}

void *
fsd_client_thread(void *data)
{	
	int err, opt;
	char buff[1024];
	fsd_session_t *session = (fsd_session_t *)data;
	pthread_detach(pthread_self());
	
	fs_debug(session->log, " ");
	fs_debug(session->log, "Client connected (ipaddr=%s)", fsd_session_get_ipaddr(session));
	
	memset(buff, 0, 1024);
	
	
	
	if (fsd_http_readline(session->btp->fd, 1024, (char *)&buff) > 0) {
		
		if (strncmp(buff, "FSTP", 4) == 0) {
			/** FSTP */
			err = fsd_fstp_handler(session, (const char *)buff);
			
		} else if ( strncmp(buff, "GET", 3) == 0 	||
			strncmp(buff, "POST", 4) == 0 		||
			strncmp(buff, "PUT", 3) == 0 		||
			strncmp(buff, "HEAD", 4) == 0 		||
			strncmp(buff, "DELETE", 6) == 0 	||
			strncmp(buff, "CONNECT", 6) == 0 	||
			false
			) {
			/** HTTP */
			err = fsd_http_handler(session, (const char *)buff);
		
		} else {
			err = ERR_PROTO;
		}
		
		if (err != ERR_OK)
			fs_debug(session->log, "Hanlder exception (err=%d), %s", err, fs_strerror(err));
	}
	
	fs_debug(session->log, "Client disconnected (ipaddr=%s)", fsd_session_get_ipaddr(session));
	
	fsd_session_fini(session);
	
	return (NULL);
}