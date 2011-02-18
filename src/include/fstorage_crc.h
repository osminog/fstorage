/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_FSTORAGE_CRC_
#define	_FSTORAGE_CRC_

#include <stdio.h>
#include <string.h>

void fs_make_crc_table(void);
unsigned long fs_update_crc(unsigned long crc, unsigned char *buf, int len);
unsigned long fs_crc(unsigned char *buf, int len);

#endif	/* !_FSTORAGE_CRC_ */