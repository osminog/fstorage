/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_CFG_H_
#define _FSTORAGE_CFG_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef _FSTORAGE_CLIENT_PHP_H_
#include <regex.h>
#endif

#include "fstorage_str.h"
#include "fstorage_errcode.h"

#define	CFG_VARS_MAX		1024
#define	CFG_NAME_LEN_MAX	64
#define	CFG_VALUE_LEN_MAX	1024
#define	CFG_LINE_LEN_MAX 	1024 + 64 + 1
#define	CFG_PATTERN  		".*=.*"

typedef struct s_cfg_var {
        char name[256];
        char value[1024];
} cfg_var_t;

typedef struct s_cfg {
        int varno;
        cfg_var_t *vars[CFG_VARS_MAX];
} cfg_t;

/* CONFIG */
int cfg_init(cfg_t **cfgp);
void cfg_fini(cfg_t *cfg);

int cfg_read(cfg_t *cfg, FILE *fd);
int cfg_read_file(cfg_t *cfg, const char *filepath);

int cfg_get(cfg_t *cfg, const char *name, char *valuep);
int cfg_varno(cfg_t *cfg);

/* CONFIG VARIABLE */
int cfg_var_init(cfg_var_t **varp);
void cfg_var_fini(cfg_var_t *var);
void cfg_vars_fini(cfg_var_t **vars);

#endif  /* !_FSTORAGE_CONF_H_ */
