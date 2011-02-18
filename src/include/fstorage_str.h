/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_STR_H_
#define _FSTORAGE_STR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "fstorage_errcode.h"

bool str_charin(char c, const char *chars);
int str_rstrip(const char *str, const char *chars, char *resultp);
int str_lstrip(const char *str, const char *chars, char *resultp);
int str_strip(const char *str, const char *chars, char *resultp);
int str_split(const char *str, const char *pattern, unsigned int *cnt, char ***strs);

void str_free(char *str);
void strs_free(char **strs);

int str2upper(const char *str, char *resultp);
int str2lower(const char *str, char *resultp);

int str2int(const char *str, int *resultp);
int str2uint64_t(const char *str, uint64_t *resultp);
int str2bool(const char *str, bool *resultp);

#endif  /* !_FSTORAGE_STR_H_ */
