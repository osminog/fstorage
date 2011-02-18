/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_cfg.h"


/* CONFIG */
static __inline int
cfg_parse_line(const char *line, cfg_var_t  **varp)
{        
        int err;
        unsigned int cnt;
        char **tab;
        
        if ((err = str_split(line, "=", &cnt, &tab)) != ERR_OK)
		return (err);
	
	if (cnt == 2) {
		if ((err = cfg_var_init(varp)) == ERR_OK) {
			str_strip(tab[0], " \t\n", (*varp)->name);
			str_strip(tab[1], " \t\n", (*varp)->value);
			//fprintf(stderr, "'%s' = '%s'\n", (*varp)->name, (*varp)->value);
		}
	} else {
		err = ERR_INVALID;
	}
        
	//strs_free(tab);
	
	return (err);
}

int
cfg_init(cfg_t **cfgp)
{
        
        if ((*cfgp = malloc(sizeof(cfg_t))) == NULL)
                return (ERR_MEMORY);
        
        (*cfgp)->varno = 0;
        memset((*cfgp)->vars, 0, CFG_VARS_MAX);
        
        return (ERR_OK);
}

void
cfg_fini(cfg_t *cfg)
{
        
        if (cfg == NULL)
                return;
        
        cfg_vars_fini(cfg->vars);
	
        free(cfg);
}

int
cfg_read(cfg_t *cfg, FILE *fd)
{
        size_t i;
        char line[CFG_LINE_LEN_MAX];
        cfg_var_t *var;
        
        regex_t reg;
        regmatch_t match;
        
        if (fd == NULL)
                return (ERR_FILESYSTEM);
        
        if (regcomp(&reg, CFG_PATTERN, REG_EXTENDED) != ERR_OK)
                return (ERR_REGEXP);
        
        while (fgets(line, sizeof(line), fd) != NULL) {
                i = strspn(line, " \t\n\v");
                if (    (line[i] != '#')                                                &&
                        (strlen(line) > 2)                                              &&
                        (regexec(&reg, line, (size_t)1, &match, 0) == ERR_OK)           &&
                        (cfg_parse_line(line, &var) == ERR_OK)                         	&&
                        true) {
                        cfg->vars[cfg->varno++] = var;
                } 
        }
         
        return (ERR_OK);
}


int
cfg_read_file(cfg_t *cfg, const char *filepath)
{
        FILE *fd = fopen(filepath, "r");
        
        return cfg_read(cfg, fd);
}

int
cfg_varno(cfg_t *cfg)
{
	
	return (cfg->varno);
}

int
cfg_get(cfg_t *cfg, const char *name, char *valuep)
{
	
	for (int i = 0; i < cfg_varno(cfg); i++) {
		
		//fprintf(stderr, "get_arg -> name = %s\n", cfg->vars[i]->name);
		
                if ((strcmp(cfg->vars[i]->name, name) == ERR_OK)) {
			
			strncpy(valuep, cfg->vars[i]->value, strlen(cfg->vars[i]->value) + 1);
			valuep[strlen(cfg->vars[i]->value)] = '\0';
			
                        return (ERR_OK);
		}
        }
        
        return (ERR_NO_RESULT);	
}

int
cfg_var_init(cfg_var_t **varp)
{
	
        if ((*varp = malloc(sizeof(cfg_var_t))) == NULL)
                return (ERR_MEMORY);
        
        return (ERR_OK);
}

void
cfg_var_fini(cfg_var_t *var)
{
        
        if (var == NULL)
                return;
        
        free(var);
}

void
cfg_vars_fini(cfg_var_t **vars)
{
        
        if (vars == NULL)
                return;
        
        for (unsigned int i = 0; vars[i] != NULL; i++)
                cfg_var_fini(vars[i]);
}
