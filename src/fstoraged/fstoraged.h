/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGED_H_
#define _FSTORAGED_H_

#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <netdb.h>

#include "fstorage_impl.h"
#include "fstoraged_conf.h"
#include "fstoraged_session.h"

#include "fstorage_mime_types.h"

#define	FSD_VERSION	"0.5"

void fsd_usage_print(void);
void fsd_version_print(void);

#endif  /* !_FSTORAGED_H_ */

