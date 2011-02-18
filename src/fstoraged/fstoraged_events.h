/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGED_EVENTS_H_
#define	_FSTORAGED_EVENTS_H_

#include <sys/queue.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct s_fsd_evt {
	uint16_t type;
	uint32_t size;
} fsd_evt_t;

struct simplehead *fsd_equeue_headp;

SIMPLEQ_HEAD(simplehead, fsd_equeue_entry) fsd_equeue_head;

struct fsd_equeue_entry {
	fsd_evt_t *evt;
	SIMPLEQ_ENTRY(fsd_equeue_entry) entries;
};

/** EVENTS */
int fsd_evt_create(uint16_t type, unsigned char *data, uint32_t data_size, fsd_evt_t **evt);
void fsd_evt_fini(fsd_evt_t *evt);

/** EVENTS QUEUE */
void fsd_equeue_init(void);
void fsd_equeue_fini(void);
uint32_t fsd_equeue_size(void);
bool fsd_equeue_empty(void);
int fsd_equeue_insert(fsd_evt_t *evt);
fsd_evt_t *fsd_equeue_pop(void);

#endif	/* !_FSTORAGED_EVENTS_H_ */
