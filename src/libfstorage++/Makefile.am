AM_CPPFLAGS = -I.
AM_CPPFLAGS += -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/src/include
AM_CPPFLAGS += -I$(top_srcdir)/src/common
AM_CPPFLAGS += -I$(top_srcdir)/src/libfstorage
AM_CPPFLAGS += -I$(top_srcdir)/src/libfstorage++

include_HEADERS = $(top_srcdir)/src/include/fstorage++.h
	
lib_LTLIBRARIES = libfstorage++.la
libfstorage___la_SOURCES = fstorage.cpp fstorage_except.cpp
libfstorage___la_LIBADD = $(top_srcdir)/src/libfstorage/libfstorage.la

