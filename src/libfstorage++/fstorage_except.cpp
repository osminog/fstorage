/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_impl.h"
#include "fstorage++.h"

/**
 * Constructor (1).
 */
FstorageExcept::FstorageExcept(int c_code)
{

	this->code = c_code;
	this->message = string(fstorage_strerror(c_code));
}

/**
 * Constructor (2).
 */
FstorageExcept::FstorageExcept(const string c_message)
{

	this->code = 0;
	this->message = c_message;
}

/**
 * Destructor.
 */
FstorageExcept::~FstorageExcept(void) throw()
{

}

/**
 * Getting exception code.
 * @return:
 *	code as integer
 */
int
FstorageExcept::get_code(void)
{

	return (this->code);
}

/**
 * Getting exception message.
 * @return:
 *	message as string
 */
string
FstorageExcept::get_message(void)
{

	return (this->message);
}
