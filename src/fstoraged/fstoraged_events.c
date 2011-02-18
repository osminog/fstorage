/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include <stdlib.h>
#include <stdio.h>
#include "fstorage_errcode.h"
#include "fstoraged_events.h"


/** EVENTS */
int
fsd_evt_create(uint16_t type, unsigned char *data, uint32_t data_size, fsd_evt_t **evt)
{
	unsigned char *ptr;
	
	if ((*evt = malloc(sizeof(fsd_evt_t) + data_size)) == NULL)
		return (ERR_MEMORY);
	
	(*evt)->type = type;
	(*evt)->size = data_size;
	
	if (data != NULL && data_size > 0) {
		ptr = (unsigned char *)(*evt);
		ptr += sizeof(fsd_evt_t);
		memcpy(ptr, data, data_size);
	}
	
	return (ERR_OK);
}

void
fsd_evt_fini(fsd_evt_t *evt)
{
	
	if (evt == NULL)
		return;
	
	free(evt);
}

/** EVENTS QUEUE */
void
fsd_equeue_init(void)
{
	
	SIMPLEQ_INIT(&fsd_equeue_head);
}

void
fsd_equeue_fini(void)
{
	struct fsd_equeue_entry *entry;
	
	while ((entry = SIMPLEQ_FIRST(&fsd_equeue_head)) != NULL) {
		fsd_evt_fini(entry->evt);
		SIMPLEQ_REMOVE_HEAD(&fsd_equeue_head, entries);
	}
	
	return;
}

uint32_t
fsd_equeue_size(void)
{
	
	return (0);
}

bool
fsd_equeue_empty(void)
{
	
	return (SIMPLEQ_EMPTY(&fsd_equeue_head));
}

int
fsd_equeue_insert(fsd_evt_t *evt)
{
	struct fsd_equeue_entry *entry;
	
	if ((entry = (struct fsd_equeue_entry*)malloc(sizeof(struct fsd_equeue_entry))) == NULL)
		return (ERR_MEMORY);
	
	entry->evt = evt;
	SIMPLEQ_INSERT_TAIL(&fsd_equeue_head, entry, entries);
	
	return (ERR_OK);
}

fsd_evt_t *
fsd_equeue_pop(void)
{
	struct fsd_equeue_entry *entry;
	fsd_evt_t *evt = NULL;
	
	if ((entry = SIMPLEQ_FIRST(&fsd_equeue_head)) != NULL) {
		evt = entry->evt;
		SIMPLEQ_REMOVE_HEAD(&fsd_equeue_head, entries);
	}
	
	return (evt);
}
