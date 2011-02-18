/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_client_php.h"

using namespace std;

zend_object_handlers fstorage_object_handlers;
zend_class_entry *fs_ce;

struct fs_object {
    zend_object std;
    Fstorage *fs;
};

void
fstorage_free_storage(void *object TSRMLS_DC)
{
	fs_object *obj = (fs_object *)object;
	
	delete obj->fs;
	
	zend_hash_destroy(obj->std.properties);
	FREE_HASHTABLE(obj->std.properties);
	
	efree(obj);
}

zend_object_value
fstorage_create_handler(zend_class_entry *type TSRMLS_DC)
{
	zval *tmp;
	zend_object_value retval;
	
	fs_object *obj = (fs_object *)emalloc(sizeof(fs_object));
	memset(obj, 0, sizeof(fs_object));
	obj->std.ce = type;
	
	ALLOC_HASHTABLE(obj->std.properties);
	zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(obj->std.properties, &type->default_properties,
	(copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(obj, NULL, fstorage_free_storage, NULL TSRMLS_CC);
    	retval.handlers = &fstorage_object_handlers;
	
	return (retval);
}

PHP_METHOD(Fstorage, __construct)
{
	int debug = 0;
	char *filepath = NULL, *fsuri = NULL;
	Fstorage *fs = NULL;
	int filepath_len = 0, fsuri_len = 0;
	zval *object = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls", &fsuri, &fsuri_len, &debug, &filepath, &filepath_len) == FAILURE) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
	try {
		fs = new Fstorage(string(fsuri), debug, filepath != NULL ? string(filepath) : string());
	} catch (FstorageExcept e) {
		zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
		RETURN_NULL();
	} catch (exception e) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
	fs_object *obj = (fs_object *)zend_object_store_get_object(object TSRMLS_CC);
	obj->fs = fs;
}

PHP_METHOD(Fstorage, file_send)
{	
	char *fkey = NULL, *filepath = NULL;
	int fkey_len = 0, filepath_len = 0;
	zval *object = getThis();
	fs_object *obj = (fs_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &fkey, &fkey_len, &filepath, &filepath_len) == FAILURE) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
	try {
		obj->fs->file_send(string(fkey), string(filepath));
	} catch (FstorageExcept e) {
		zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
		RETURN_NULL();
	} catch (exception e) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
	RETURN_NULL();
}

PHP_METHOD(Fstorage, file_recv)
{
	char *fkey = NULL, *filepath = NULL;
	int fkey_len, filepath_len;
	fs_object *obj;
	size_t size;
	zval *object = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &fkey, &fkey_len, &filepath, &filepath_len) == FAILURE) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
	obj = (fs_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	try {
		size = obj->fs->file_recv(string(fkey), string(filepath));
	} catch (FstorageExcept e) {
		zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
		RETURN_NULL();
	} catch (exception e) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
	RETURN_LONG(size);
}

PHP_METHOD(Fstorage, file_replace)
{
	char *fkey = NULL, *filepath = NULL;
	int fkey_len = 0, filepath_len = 0;
	zval *object = getThis();
	fs_object *obj = (fs_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &fkey, &fkey_len, &filepath, &filepath_len) == FAILURE) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
	try {
		obj->fs->file_replace(string(fkey), string(filepath));
	} catch (FstorageExcept e) {
		zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
		RETURN_NULL();
	} catch (exception e) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
	RETURN_NULL();
}

PHP_METHOD(Fstorage, file_remove)
{
	char *fkey;
	int fkey_len;
	fs_object *obj;
	zval *object = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fkey, &fkey_len) == FAILURE) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
	obj = (fs_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	try {
		obj->fs->file_remove(string(fkey));
	} catch (FstorageExcept e) {
		zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
		RETURN_NULL();
	} catch (exception e) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
	RETURN_NULL();
}

PHP_METHOD(Fstorage, hash)
{
	char *key = NULL;
	int key_len;
	string hash = string();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
	try {
		hash = Fstorage::hash(string(key));
	} catch (FstorageExcept e) {
		zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
		RETURN_NULL();
	} catch (exception e) {
		RETURN_NULL();
	}
	
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
	RETURN_STRING((char *)hash.c_str(), true);
}

PHP_METHOD(Fstorage, http_url)
{
	char *filename = NULL, *fkey = NULL, *akey = NULL;
        int filename_len, fkey_len, akey_len;
	string url = string();
        fs_object *obj;
        zval *object = getThis();
	
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s!",
		&filename, &filename_len, &fkey, &fkey_len, &akey, &akey_len) == FAILURE) {
                RETURN_NULL();
        }
	
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
        obj = (fs_object *)zend_object_store_get_object(object TSRMLS_CC);
	
        try {
                url = obj->fs->http_url(string(filename), string(fkey), akey != NULL ? string(akey) : string());
        } catch (FstorageExcept e) {
                zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
                RETURN_NULL();
        } catch (exception e) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
	RETURN_STRING((char *)url.c_str(), true);
}

PHP_METHOD(Fstorage, https_url)
{
	char *filename = NULL, *fkey = NULL, *akey = NULL;
        int filename_len, fkey_len, akey_len;
        string url = string();
        fs_object *obj;
        zval *object = getThis();
	
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s!",
                &filename, &filename_len, &fkey, &fkey_len, &akey, &akey_len) == FAILURE) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
        obj = (fs_object *)zend_object_store_get_object(object TSRMLS_CC);
	
        try {
                url = obj->fs->https_url(string(filename), string(fkey), akey != NULL ? string(akey) : string());
        } catch (FstorageExcept e) {
                zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
                RETURN_NULL();
        } catch (exception e) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
        RETURN_STRING((char *)url.c_str(), true);
}

PHP_METHOD(Fstorage, uri)
{	
	char *filename = NULL, *fkey = NULL, *akey = NULL;
        int filename_len, fkey_len, akey_len = 0;
        string uri = string();
	
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s!",
                &filename, &filename_len, &fkey, &fkey_len, &akey, &akey_len) == FAILURE) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
        try {
                uri = Fstorage::uri(string(filename), string(fkey), akey != NULL ? string(akey) : string());
        } catch (FstorageExcept e) {
                zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
                RETURN_NULL();
        } catch (exception e) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
        RETURN_STRING((char *)uri.c_str(), true);
}

PHP_METHOD(Fstorage, ns_encode)
{
        uint64_t num;
	int base;
        string ciphertext = string();
	
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &num, &base) == FAILURE) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
        try {
                ciphertext = Fstorage::ns_encode(num, base);
        } catch (FstorageExcept e) {
                zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
                RETURN_NULL();
        } catch (exception e) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
        RETURN_STRING((char *)ciphertext.c_str(), true);
}

PHP_METHOD(Fstorage, ns_decode)
{
	char *ciphertext;
	uint64_t num;
	int ciphertext_len, base;
	
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &ciphertext, &ciphertext_len, &base) == FAILURE) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
        try {
               	num = Fstorage::ns_decode(string(ciphertext), base);
        } catch (FstorageExcept e) {
                zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
                RETURN_NULL();
        } catch (exception e) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
        RETURN_LONG(num);
}

PHP_METHOD(Fstorage, akey_create)
{
	char *fkey = NULL;
	uint32_t expire_time;
	uint64_t salt;
        int fkey_len, base;
        string akey = string();
	
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsll",
                &expire_time, &fkey, &fkey_len, &salt, &base) == FAILURE) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
        try {
                akey = Fstorage::akey_create(expire_time, string(fkey), salt, base);
        } catch (FstorageExcept e) {
                zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
                RETURN_NULL();
        } catch (exception e) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
        RETURN_STRING((char *)akey.c_str(), true);
}

PHP_METHOD(Fstorage, akey_check)
{
	char *fkey = NULL, *akey = NULL;
        uint64_t salt;
        int fkey_len, akey_len, base;
	bool akey_ok = false;
	
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",
                &akey, &akey_len, &fkey, &fkey_len, &salt, &base) == FAILURE) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	
        try {
                akey_ok = Fstorage::akey_check(string(akey), string(fkey), salt, base);
        } catch (FstorageExcept e) {
                zend_throw_exception(zend_exception_get_default(), (char *)e.get_message().c_str(), e.get_code() TSRMLS_CC);
                RETURN_NULL();
        } catch (exception e) {
                RETURN_NULL();
        }
	
        php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
	if (akey_ok) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

function_entry fstorage_methods[] = {
	PHP_ME(Fstorage, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Fstorage, file_send, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Fstorage, file_recv, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Fstorage, file_replace, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Fstorage, file_remove, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Fstorage, hash, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(Fstorage, http_url, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Fstorage, https_url, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Fstorage, uri, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(Fstorage, ns_encode, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(Fstorage, ns_decode, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(Fstorage, akey_create, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(Fstorage, akey_check, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(fstorage)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Fstorage", fstorage_methods);
	
	fs_ce = zend_register_internal_class(&ce TSRMLS_CC);
	fs_ce->create_object = fstorage_create_handler;
	
	memcpy(&fstorage_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	fstorage_object_handlers.clone_obj = NULL;
	
	return (SUCCESS);
}

zend_module_entry fstorage_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_FSTORAGE_EXT_NAME,
	NULL,			/* Functions */
	PHP_MINIT(fstorage),	/* MINIT */
	NULL,			/* MSHUTDOWN */
	NULL,			/* RINIT */
	NULL,			/* RSHUTDOWN */
	NULL,			/* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
	PHP_FSTORAGE_EXT_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FSTORAGE
extern "C" {
ZEND_GET_MODULE(fstorage)
}
#endif

