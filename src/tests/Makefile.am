# -
# tests
# Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
# - 

AM_CPPFLAGS = -I. -I$(top_srcdir)/src/include

bin_PROGRAMS = test_libfstorage.t test_crc.t

EXTRA_DIST = misc.h

test_libfstorage_t_SOURCES = test_libfstorage.c misc.c
test_libfstorage_t_LDADD = $(top_srcdir)/src/libfstorage/libfstorage.la

test_crc_t_SOURCES = test_crc.c misc.c
test_crc_t_LDADD = $(top_srcdir)/src/libfstorage/libfstorage.la
