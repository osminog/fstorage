/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#ifndef _FSTORAGE_CPP_H_
#define _FSTORAGE_CPP_H_

#include <string>
#include <exception>
#include <stdexcept>

extern "C" {
#include <fstorage.h>
}

using std::string;

/**
 * FstorageExcept class.
 */
class FstorageExcept: public std::exception
{
private:
	string message;
	int code;
	
public:
	FstorageExcept(int c_code);
	FstorageExcept(const string c_message);
	virtual ~FstorageExcept(void) throw();
	string get_message(void);
	int get_code(void);
	
};

/**
 * Fstorage C++ interface.
 */
class Fstorage
{
private:
	fs_ctx_t *ctx;
	
public:
	Fstorage(const string fsuri, unsigned int debug = 0, const string logfile = string());
        ~Fstorage(void);
	
	void file_send(const string fkey, const string filepath);
	size_t file_recv(const string fkey, const string filepath);
	void file_replace(const string fkey, const string filepath);
	void file_remove(const string fkey);

	static string hash(const string key);
	
	string http_url(const string filename, const string fkey, const string akey = string());
	string https_url(const string filename, const string fkey, const string akey = string());
	
	static string ns_encode(uint64_t num, uint8_t base);
	static uint64_t ns_decode(const string ciphertext, uint8_t base);
	static string akey_create(uint32_t expire_time, const string fkey, uint64_t salt, uint8_t base);
	static bool akey_check(const string akey, const string fkey, uint64_t salt, uint8_t base);
	static string uri(const string filename, const string fkey, const string akey = string());
	
};

#endif /* !_FSTORAGE_CPP_H_ */
