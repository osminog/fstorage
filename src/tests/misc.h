/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef	_MISC_H_
#define	_MISC_H_

#include <setjmp.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fstorage_errcode.h"

void install_abort_handler(void);
void deinstall_abort_handler(void);

#define	EXPECT_INIT(_testcount)						\
	int testno = 1;							\
	printf("1..%d\n", _testcount);

#define EXPECT(e, expected)	do {					\
	int _error = (e);						\
	if (_error == expected) 					\
		printf("ok %d\n", testno);				\
	else								\
		printf("not ok %d - %s failed with code: %d\n",		\
		    testno, #e, _error);			        \
	testno++;							\
} while (0)

#define EXPECT_OK(e) EXPECT(e, ERR_OK)

#define EXPECT_STR(e, expected)	do {					\
	const char *result = (e);					\
	if (strcmp(result, expected) == 0)				\
		printf("ok %d\n", testno);				\
	else								\
		printf("not ok %d - %s failed; expected '%s',"		\
		    " got '%s'\n", testno, #e, expected, result);	\
	testno++;							\
} while (0)

#define EXPECT_OR_RETURN(e, expected)	do {				\
	int _error = (e);						\
	if (_error == expected) 					\
		printf("ok %d\n", testno);				\
	else {								\
		printf("not ok %d - %s failed with code: %d\n",	        \
		    testno, #e, _error);			        \
		return (1);						\
	}								\
	testno++;							\
} while (0)

#define EXPECT_OK_OR_RETURN(e) EXPECT_OR_RETURN(e, ERR_OK)

#define EXPECT_ASSERT(e)	do {					\
	install_abort_handler();					\
	if (setjmp(jump))						\
		printf("ok %d\n", testno);				\
	else {								\
		(e);							\
		printf("not ok %d - %s didn't trip an assert.\n",	\
		    testno, #e);					\
	}								\
	deinstall_abort_handler();					\
	testno++;							\
} while (0)

#endif	/* !_MISC_H_ */
