/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstoraged_conf.h"

int
fsd_conf_init(const char *filepath, fsd_conf_t **cfgp)
{
	int err, debug, base, port;
	unsigned int cnt;
	cfg_t *mcfg = NULL;
	fs_log_t *log = NULL;
	char value[CFG_VALUE_LEN_MAX], host[128], path[256], logfile[256];
	uint64_t salt;
	
	err = fs_log_init(NULL, &log);
	
	if (err == ERR_OK && (err = cfg_init(&mcfg)) != ERR_OK)
		fs_error(log, "[ CONFIG ] Cannot initialize 'cfg'");
	
	if (err == ERR_OK && (err = cfg_read_file(mcfg, filepath)) != ERR_OK) {
		if (filepath != NULL)
			fs_error(log, "[ CONFIG ] Cannot load config file '%s'", filepath);
	}
	
	if (err == ERR_OK && (*cfgp = malloc(sizeof(fsd_conf_t))) == NULL)
		err = ERR_MEMORY;
	
	/* Checking for debug variable */
	if (err == ERR_OK && (err = cfg_get(mcfg, "debug", (char *)&value)) != ERR_OK)
		fs_error(log, "[ CONFIG ] Cannot find 'debug' variable in config file");
	
	if (err == ERR_OK && (err = str2int(value, &debug)) != ERR_OK)
		fs_error(log, "[ CONFIG ] Invalid Type of 'debug' = '%s'. Cardinal is required", value);
	
	if (err == ERR_OK)
		(*cfgp)->debug = debug;
	
	/* Checking for port variable */
	if (err == ERR_OK && (err = cfg_get(mcfg, "port", (char *)&value)) != ERR_OK)
		fs_error(log, "[ CONFIG ] Cannot find 'port' variable in config file");

	if (err == ERR_OK && (err = str2int(value, &port)) != ERR_OK)
		fs_error(log, "[ CONFIG ] Invalid Type of 'port' = '%s'. Cardinal is required", value);
	
	if (err == ERR_OK && (port < 2 || port > (1 << 16))) {
		fs_error(log, "[ CONFIG ] variable 'port' = %d is out of range <1, %d>", port, (1 << 16));
		err = ERR_CONFIG;
	}
	
	if (err == ERR_OK)
		(*cfgp)->port = (uint16_t)port;
	
	/* Checking for host variable */
	if (err == ERR_OK && (err = cfg_get(mcfg, "host", (char *)&host)) != ERR_OK)
		fs_error(log, "[ CONFIG ] Cannot find 'host' variable in config file");
	
	if (err == ERR_OK)
		strncpy((*cfgp)->host, host, strlen(host) + 1);
	
	/* Checking for path variable */
	if (err == ERR_OK && (err = cfg_get(mcfg, "path", (char *)&path)) != ERR_OK)
		fs_error(log, "[ CONFIG ] Cannot find 'path' variable in config file");
	
	if (err == ERR_OK && !fs_dir_exist(path)) {
		fs_error(log, "[ CONFIG ] 'path' = '%s' not exist or no permission", path);
		err = ERR_FILESYSTEM;
	}
	
	if (err == ERR_OK)
		strncpy((*cfgp)->path, path, strlen(path) + 1);
	
	/* Checking for logfile variable */
	if (err == ERR_OK && cfg_get(mcfg, "logfile", (char *)&logfile) == ERR_OK) {
		strncpy((char *)&(*cfgp)->logfile, logfile, strlen(logfile) + 1);
		(*cfgp)->logfile[strlen(logfile)] = '\0';
	} else if (err == ERR_OK) {
		(*cfgp)->logfile[0] = '\0';
	}
	
	/* Checking for salt variable */
	if (err == ERR_OK && cfg_get(mcfg, "salt", (char *)&value) == ERR_OK) {
		if (err == ERR_OK && (err = str2uint64_t(value, &salt)) != ERR_OK)
			fs_error(log, "[ CONFIG ] Invalid Type of 'salt' = '%s'. Long cardinal is required", value);
		else
			(*cfgp)->salt = salt;
	} else 
		(*cfgp)->salt = FS_DEFAULT_SALT;
		
	/* Checking for base variable */
	if (err == ERR_OK && cfg_get(mcfg, "base", (char *)&value) == ERR_OK) {
		if (err == ERR_OK && (err = str2int(value, &base)) != ERR_OK)
			fs_error(log, "[ CONFIG ] Invalid Type of 'base' = '%s'. Cardinal is required", value);
		else {
			if (err == ERR_OK && (base < 2 || base > 62)) {
				fs_error(log, "[ CONFIG ] variable 'base' = %d is out of range <2, 62>", base);
				err = ERR_CONFIG;
			} else 
				(*cfgp)->base = (uint8_t)base;
		}			
	} else
		(*cfgp)->base = FS_DEFAULT_BASE;
	
	/* Checking form allowed_ips variable*/
	cnt = 0;
	if (err == ERR_OK && cfg_get(mcfg, "allowed_ips", (char *)&value) == ERR_OK) {
		if (str_split(value, ",", &cnt, &((*cfgp)->allowed_ips)) == ERR_OK) {
			for (unsigned int j = 0; j < cnt; j++) {
				str_strip((*cfgp)->allowed_ips[j], " \n\t", (char *)&value);
				strncpy((*cfgp)->allowed_ips[j], value, strlen(value));
				(*cfgp)->allowed_ips[j][strlen(value)] = '\0';
			}
		}
	}
	
	if (err == ERR_OK)
		(*cfgp)->allowed_ips_cnt = cnt;
	
	cfg_fini(mcfg);
	fs_log_fini(log);
	
	return (err);
}

void
fsd_conf_fini(fsd_conf_t *cfg)
{
	
	if (cfg == NULL)
		return;
	
	free(cfg);
}

