/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */


#include "fstorage_impl.h"
#include "fstorage_backend.h"
#include "fstorage_context.h"

int
fstorage_backend_init(const char *proto, const char *name, const char *host, uint16_t port, fs_backend_t **backend)
{
	
	if ((*backend = malloc(sizeof(fs_backend_t))) == NULL)
		return (ERR_MEMORY);
	
	str_strip(proto, " ", (char *)&(*backend)->proto);
	str_strip(name, " ", (char *)&(*backend)->name);
	str_strip(host, " ", (char *)&(*backend)->host);
	
	(*backend)->port = port;
	
	return (ERR_OK);
}

void
fstorage_backend_fini(fs_backend_t *backend)
{
	
	if (backend == NULL)
		return;
	
	free(backend);
}

void
fstorage_backends_fini(fs_backend_t **backends)
{
	
	if (backends == NULL)
		return;
	
	for (unsigned int i = 0; backends[i] != NULL; i++)
		fstorage_backend_fini(backends[i]);
}

int
fstorage_backend_parse(const char *str, fs_backend_t **backendp)
{
	int err, port;
	unsigned int cnt1, cnt2, cnt3;
	char tmp1[256], tmp2[256], **tab1 = NULL, **tab2 = NULL;
	char proto[16], name[128], host[128];
	
	err = ERR_INVALID;
	str_split(str, "://", &cnt1, &tab1);
	
	if (cnt1 == 2) {
		str_strip(tab1[0], " \t", (char *)&proto);
		
		if (strcmp(proto, "fstp") == 0 || strcmp(proto, "http") == 0) {
			str_strip(tab1[1], " \t", (char *)&tmp1);
			str_split(tmp1, ":", &cnt2, &tab2);
			
			if (cnt2 == 2) {
				str_strip(tab2[0], " \t", (char *)&host);
				str_strip(tab2[1], " \t", (char *)&tmp2);
			} else {
				str_strip(tab2[0], " \t", (char *)&host);
				*tmp2 = "80";
			}
			
			if (str2int(tmp2, &port) == ERR_OK)
				err = fstorage_backend_init(proto, name, host, (uint16_t)port, backendp);
		}
	}
	
	return (err);
}

int
fstorage_backends_parse(const char *str, unsigned int *cnt, fs_backend_t **backendsp[])
{
	int err;
	unsigned int tmp_cnt, counter;
	char **tab;
	
	if ((err = str_split(str, ",", &tmp_cnt, &tab)) != ERR_OK)
		return (err);
	
	counter = 0;
	for (unsigned int i = 0; i < tmp_cnt; i++)
		if (fstorage_backend_parse((const char *)tab[i], (fs_backend_t **)(backendsp + counter)) == ERR_OK)
			counter++;
	
	*cnt = counter;
	
	return (ERR_OK);
}

int
fstorage_get_backend_by_name(fs_ctx_t *ctx, const char *name, unsigned int *id_backendp)
{
	
	for (unsigned int i = 0; i < ctx->cfg->backends_cnt; i++) {
		if (strcmp(ctx->cfg->backends[i]->name, name) == 0) {
			*id_backendp = i;
			return (ERR_OK);
		}
	}
	
	return (ERR_NO_RESULT);
}

int
fstorage_get_backend_shuffle(fs_ctx_t *ctx, unsigned int *id_backendp)
{
	
	if (ctx->cfg->backends_cnt == 0)
		return (ERR_NO_RESULT);
	
	if (ctx->cfg->backends_cnt == 1)
		*id_backendp = 0;
	else 
		*id_backendp = fs_numgen(ctx->cfg->backends_cnt) - 1;
	
	return (ERR_OK);
}