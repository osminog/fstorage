# -
# Copyright (c) 2009-2010 by Fstorage Team.
# Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
# -

PHP_ARG_ENABLE(fstorage, whether to enable Fstorage support,
[ --enable-fstorage   Enable Fstorage support])

if test "$PHP_FSTORAGE" = "yes"; then
	PHP_REQUIRE_CXX()
	PHP_SUBST(FSTORAGE_SHARED_LIBADD)
	PHP_ADD_LIBRARY(stdc++, 1, FSTORAGE_SHARED_LIBADD)
	PHP_ADD_LIBRARY(fstorage++, 1, FSTORAGE_SHARED_LIBADD)
	PHP_NEW_EXTENSION(fstorage, fstorage_client_php.cpp, $ext_shared)
fi
