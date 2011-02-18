AM_CPPFLAGS = -I.
AM_CPPFLAGS += -I$(top_srcdir)/src/include

EXTRA_DIST = fstoraged.h \
    fstoraged_client.h \
    fstoraged_conf.h \
    fstoraged_engine.h \
    fstoraged_events.h \
    fstoraged_file.h \
    fstoraged_fstp.h \
    fstoraged_hdr.h \
    fstoraged_http.h \
    fstoraged_session.h
    
bin_PROGRAMS = fstoraged
fstoraged_SOURCES = fstoraged.c \
	fstoraged_client.c \
	fstoraged_conf.c \
	fstoraged_session.c \
	fstoraged_fstp.c \
	fstoraged_http.c \
	fstoraged_hdr.c

fstoraged_LDADD = $(top_srcdir)/src/libfstorage/libfstorage.la -lpthread

