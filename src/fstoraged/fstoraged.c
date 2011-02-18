/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include <signal.h>

//#include "fstoraged_engine.h"
#include "fstoraged_client.h"
#include "fstoraged.h"

static const char* exec_name="";
static void
fsd_crash_handler(int sig)
{
	const char *gdb_array[] = {"gdb", exec_name, "PID", NULL};
	char pid_str[40];

	sprintf(pid_str, "%d%c", getpid(), '\0');
	gdb_array[2] = pid_str;

	const int pid = fork();

	if (pid < 0) {
		abort();
	} else if (pid) {
		sleep(60);
		_exit(1);
	} else {
		execvp("gdb", (char* const*)gdb_array);
	}
}

static void
fsd_sigpipe_handler(int sig)
{
	printf("Received SIGPIPE signal\n");
	sleep(1);
	return;
}

static void
fsd_register_gdb(void)
{

	signal(SIGQUIT,	fsd_crash_handler);
	signal(SIGILL,	fsd_crash_handler);
	signal(SIGTRAP,	fsd_crash_handler);
	signal(SIGABRT,	fsd_crash_handler);
	signal(SIGFPE,	fsd_crash_handler);
	signal(SIGBUS,	fsd_crash_handler);
	signal(SIGSEGV,	fsd_crash_handler);
	signal(SIGSYS, 	fsd_crash_handler);
}



int
main(int argc, char *argv[])
{	
	int err, opt, sock_srv, sock_cli;
	unsigned int i;
	socklen_t socklen;
	struct sockaddr_in sa;
	unsigned long a;
	static char numeric_str[32];
	fs_log_t *log;
	fsd_conf_t *cfg;
	pthread_t tid;
	uint64_t id = 0;
	char *cfgpath = NULL, *ipaddr;
	bool daemonize = false, testmode = false, allowed = true;

	if (argc > 1) {
		for (int j = 1; j < argc; j++) {
			if (strcmp(argv[j], "-d") == 0 || strcmp(argv[j], "--daemon") == 0) {
				daemonize = true;
			} else if (strcmp(argv[j], "-T") == 0) {
				testmode = true;
			} else if (strcmp(argv[j], "-c") == 0 || strcmp(argv[j], "--config") == 0) {
				cfgpath = ((j+1) < argc) ? argv[j+1] : NULL;
			} else if (strcmp(argv[j], "-h") == 0 || strcmp(argv[j], "--help") == 0) {
				fsd_usage_print();
				return (ERR_OK);
			} else if (strcmp(argv[j], "-v") == 0 || strcmp(argv[j], "--version") == 0) {
				fsd_version_print();
				return (ERR_OK);
			}
		}
	}
	
	if (cfgpath == NULL) {
		fsd_usage_print();
		return (ERR_CONFIG);
	}
	
	if ((err = fsd_conf_init(cfgpath, &cfg)) != ERR_OK)
		return (err);
	
	if ((err = fs_log_init(daemonize ? ((cfg->logfile != NULL && cfg->logfile[0] != '\0') ? (char *)&(cfg->logfile) : "/dev/null") : (char *)&(cfg->logfile), &log)) != ERR_OK)
		return (err);
	
	fs_log_set_level(log, cfg->debug);
	
	if ((sock_srv = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		fs_error(log, "Cannot create socket");
		return (ERR_SOCKET);
	}
	
#if defined(SO_REUSEADDR)	
	opt = 1;
	if (setsockopt(sock_srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		fs_error(log, "Cannot set sockopt REUSEADDR");
		return (ERR_SOCKET);
	}
#endif
	
#if defined(SO_REUSEPORT)
	opt = 1;
	if (setsockopt(sock_srv, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)  ) < 0) {
		fs_error(log, "Cannot set sockopt REUSEPORT");
		return (ERR_SOCKET);
	}
#endif


/*
#if defined(SO_KEEPALIVE)
	opt = 1;
	if(setsockopt(sock_srv, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
		fs_error(log, "Cannot set sockopt KEEPALIVE");
		return (ERR_SOCKET);
	}
#endif
*/

	memset(&sa, 0, sizeof(sa));
	
	sa.sin_family = AF_INET;
	sa.sin_port  = htons(cfg->port);
	sa.sin_addr.s_addr = inet_addr(cfg->host);
	
	if (bind(sock_srv, (struct sockaddr *)&sa, sizeof sa) < 0) {
		fs_error(log, "Cannot bind addr '%s:%d' to socket", cfg->host, cfg->port);
		return (ERR_BIND);
	}
	
	fs_info(log, "Server started on '%s:%d'", cfg->host, cfg->port);
	fs_info(log, "[cfg] path = '%s'", cfg->path);
	fs_info(log, "[cfg] salt = %lu (%s)", cfg->salt, cfg->salt == FS_DEFAULT_SALT ? "default" : "custom");
	fs_info(log, "[cfg] base = %d (%s)", cfg->base, cfg->base == FS_DEFAULT_BASE ? "default" : "custom");
	
	if (daemonize) {
		switch (fork()) {
		case -1:
			fs_error(log, "Cannot creat new process");
			return (ERR_FORK);
			break;
		default:
			close(sock_srv);
			return (ERR_OK);
			break;
		case 0:
			break;
		}
	}
	
	listen(sock_srv, 5);
	
	/** RUNING ENGINE (autonomy thread)*/
	//fsd_engine_run(cfg, log);

	signal(SIGPIPE, fsd_sigpipe_handler);
	fsd_register_gdb();
	
	/** HANDLING CLIENTS CONNECTIONS */
	for (;;) {
		
		socklen = sizeof(sa);
		
		if ((sock_cli = accept(sock_srv, (struct sockaddr *)&sa, &socklen)) < 0) {
			fs_error(log, "Accept socket error");
			continue;
		}
		
		{
			memset((char *)numeric_str, 0, 32);
			a = ntohl(sa.sin_addr.s_addr);
			sprintf((char *)&numeric_str, "%u.%u.%u.%u",
			      (unsigned) (a >> 24) & 0xff, (unsigned) (a >> 16) & 0xff,
			      (unsigned) (a >> 8) & 0xff, (unsigned) a & 0xff);
			ipaddr = numeric_str;
			//ipaddr[strlen(ipaddr)] = '\0';
		}
		
		/*
		if (cfg->allowed_ips_cnt > 0) {
			i = 0;
			allowed = false;
			while(i < cfg->allowed_ips_cnt) {
				if (strcmp(ipaddr, cfg->allowed_ips[i]) == 0) {
					allowed = true;
					break;
				}
				i++;
			}
		}
		
		
		if (allowed) {
			fsd_client(cfg, log, sock_cli, ipaddr);
		} else {
			close(sock_cli);
		}
		
		*/
		
		//fs_info(log, " ------------ IP %s ------------", (const char *)ipaddr);
		
		
		if ((err = fsd_client(cfg, log, sock_cli, (const char *)ipaddr)) != ERR_OK) {
			fs_warn(log, "Client process faild (err=%d), %s", err, fs_strerror(err));
			close(sock_cli);
		}
		
	} // end of while
	
	fprintf(stdout, "Usage: fstoraged [options]\n");
	
	close(sock_srv);
	
	return (ERR_OK);
}

void
fsd_usage_print(void)
{
	
	fprintf(stdout, "Usage: fstoraged [options]\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Options:\n");
	fprintf(stdout, "  -v, --version          \tshow program's version number and exit\n");
	fprintf(stdout, "  -h, --help             \tshow this help message and exit\n");
	fprintf(stdout, "  -d, --daemon           \trun in background mode\n");
	fprintf(stdout, "  -c PATH, --config PATH \tpath to config file\n");
	fflush(stdout);
}

void
fsd_version_print(void)
{
	
	fprintf(stdout, "version = %s\n", FSD_VERSION);
	fflush(stdout);
}
