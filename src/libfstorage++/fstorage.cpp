/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_impl.h"
#include "fstorage++.h"

/**
 * Constructor.
 * @param filepath: Path to config file
 */
Fstorage::Fstorage(const string fsuri, unsigned int debug, const string logfile)
{
	int err;
	
	if ((err = fstorage_init(fsuri.c_str(), debug, logfile.c_str(), &this->ctx)) != ERR_OK)
		throw FstorageExcept(err);
}

/**
 * Destructor.
 */
Fstorage::~Fstorage(void)
{

	fstorage_fini(this->ctx);
}

/**
 * Sending file.
 * @param fkey: file key as string
 * @param filepath: path to file
 */
void
Fstorage::file_send(const string fkey, const string filepath)
{
	int err;
	
	if ((err = fstorage_file_send(this->ctx, fkey.c_str(), filepath.c_str())) != ERR_OK)
		throw FstorageExcept(err);
	
	return;
}

/**
 * Receiving file.
 * @param fkey: file key as string
 * @param filepath: path to file
 * @return:
 *	file size as cardinal
 */
size_t
Fstorage::file_recv(const string fkey, const string filepath)
{
	int err;
	size_t size;
	
	if ((err = fstorage_file_recv(this->ctx,fkey.c_str(), filepath.c_str(), &size)) != ERR_OK)
		throw FstorageExcept(err);
	
	return (size);
}

/**
 * Replacing file.
 * @param fkey: file key as string
 * @param filepath: path to file
 */
void
Fstorage::file_replace(const string fkey, const string filepath)
{
	int err;
	
	if ((err = fstorage_file_replace(this->ctx, fkey.c_str(), filepath.c_str())) != ERR_OK)
		throw FstorageExcept(err);
	
	return;
}

/**
 * Removing file.
 * @param fkey: file key as string
 */
void
Fstorage::file_remove(const string fkey)
{
	int err;
	
	if ((err = fstorage_file_remove(this->ctx, fkey.c_str())) != ERR_OK)
		throw FstorageExcept(err);
	
	return;
}

string
Fstorage::hash(const string key)
{
	int err;
	char buff[64];
	
	if ((err = fstorage_hash(key.c_str(), (char *)&buff)) != ERR_OK)
		throw FstorageExcept(err);
	
	return (string((const char *)buff));
}

/**
 * Creating http url.
 * @param filename: name of file with extension (eg.: name.txt)
 * @param fkey: file key as string
 * @param akey: time access key as string
 * @return:
 *	full http url 
 */
string
Fstorage::http_url(const string filename, const string fkey, const string akey)
{
	int err;
	char url[1024];
	
	if ((err = fstorage_http_url(this->ctx, filename.c_str(), fkey.c_str(), akey != string() ? akey.c_str() : NULL, (char *)&url)) != ERR_OK)
		throw FstorageExcept(err);
	
	return (string((const char *)url));
}

/**
 * Creating https url.
 * @param filename: name of file with extension (eg.: name.txt)
 * @param fkey: file key as string
 * @param akey: time access key as string
 * @return:
 *	full https url 
 */
string	
Fstorage::https_url(const string filename, const string fkey, const string akey)
{
	int err;
	char url[1024];
	
	if ((err = fstorage_https_url(this->ctx, filename.c_str(), fkey.c_str(), akey != string() ? akey.c_str() : NULL, (char *)&url)) != ERR_OK)
		throw FstorageExcept(err);
	
	return (string((const char *)url));
}

string
Fstorage::ns_encode(uint64_t num, uint8_t base)
{
	int err;
	char ciphertext[256];
	
	if ((err = fstorage_ns_encode(num, base, (char *)&ciphertext)) != ERR_OK)
		throw FstorageExcept(err);
	
	return (string((const char *)ciphertext));
}

uint64_t
Fstorage::ns_decode(const string ciphertext, uint8_t base)
{
	int err;
	uint64_t num;
	
	if ((err = fstorage_ns_decode(ciphertext.c_str(), base, &num)) != ERR_OK)
		throw FstorageExcept(err);
	
	return (num);
}

string
Fstorage::akey_create(uint32_t expire_time, const string fkey, uint64_t salt, uint8_t base)
{
	int err;
	char akey[256];
	
	if ((err = fstorage_akey_create(expire_time, fkey.c_str(), salt, base, (char *)&akey)) != ERR_OK)
		throw FstorageExcept(err);
	
	return (string((const char *)akey));
}

bool
Fstorage::akey_check(const string akey, const string fkey, uint64_t salt, uint8_t base)
{
	
	return (fstorage_akey_check(akey.c_str(), fkey.c_str(), salt, base));
}

string
Fstorage::uri(const string filename, const string fkey, const string akey)
{
	int err;
	char uri[1024];
	
	if ((err = fstorage_uri(filename.c_str(), fkey.c_str(), akey != string() ? akey.c_str() : NULL, (char *)&uri)) != ERR_OK)
		throw FstorageExcept(err);
	
	return (string((const char *)uri));
}
