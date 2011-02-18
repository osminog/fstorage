/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_NS_H_
#define _FSTORAGE_NS_H_

#include <stdint.h>

#include "fstorage_errcode.h"

int fs_ns_encode(uint64_t num, uint8_t base, char *ciphertextp);
int fs_ns_decode(const char *ciphertext, uint8_t base, uint64_t *num);

#endif  /* !_FSTORAGE_NS_H_ */
