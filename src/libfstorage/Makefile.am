AM_CPPFLAGS = -I.
AM_CPPFLAGS += -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_srcdir)/src/include
AM_CPPFLAGS += -I$(top_srcdir)/src/libfstorage
AM_CPPFLAGS += -I$(top_srcdir)/src/common
AM_CPPFLAGS += -I$(top_srcdir)/src/btp

include_HEADERS = $(top_srcdir)/src/include/fstorage.h

EXTRA_DIST = $(top_srcdir)/src/include/fstorage_backend.h \
    $(top_srcdir)/src/include/fstorage_btp.h \
    $(top_srcdir)/src/include/fstorage_cfg.h  \
    $(top_srcdir)/src/include/fstorage_common.h \
    $(top_srcdir)/src/include/fstorage_conf.h \
    $(top_srcdir)/src/include/fstorage_context.h \
    $(top_srcdir)/src/include/fstorage_crc.h \
    $(top_srcdir)/src/include/fstorage_errcode.h \
    $(top_srcdir)/src/include/fstorage_impl.h \
    $(top_srcdir)/src/include/fstorage_keys.h \
    $(top_srcdir)/src/include/fstorage_log.h \
    $(top_srcdir)/src/include/fstorage_mime_types.h \
    $(top_srcdir)/src/include/fstorage_ns.h \
    $(top_srcdir)/src/include/fstorage_opcode.h \
    $(top_srcdir)/src/include/fstorage_str.h

lib_LTLIBRARIES = libfstorage.la
libfstorage_la_SOURCES = fstorage.c \
	fstorage_conf.c \
	fstorage_backend.c \
	fstorage_context.c\
	$(top_srcdir)/src/common/str.c \
	$(top_srcdir)/src/common/keys.c \
	$(top_srcdir)/src/common/ns.c \
	$(top_srcdir)/src/common/common.c \
	$(top_srcdir)/src/common/cfg.c \
	$(top_srcdir)/src/common/log.c \
	$(top_srcdir)/src/common/crc.c \
	$(top_srcdir)/src/common/mime_types.c \
	$(top_srcdir)/src/btp/btp.c \
	$(top_srcdir)/src/btp/btp_pkg.c \
	$(top_srcdir)/src/btp/btp_arg.c \
	$(top_srcdir)/src/btp/btp_value.c
	
libfstorage_la_LIBADD = -lm
