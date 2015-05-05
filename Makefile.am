AUTOMAKE_OPTIONS = foreign subdir-objects

VERSION_INFO = 1:0:0

#include src/Makefile.am
#include test/Makefile.am

DIST_SUBDIRS = src test

SUBDIRS = src

EXTRA_DIST = LICENSE Changes powapi.m4 autogen.sh  

include_HEADERS = pow.h

lib_LTLIBRARIES = libpowapi.la

libpowapi_la_SOURCES = src/pow.cc src/init.cc src/app_hint.c
libpowapi_la_LDFLAGS = -version-info $(VERSION_INFO)

ACLOCAL_AMFLAGS = -I m4

#test_init_SOURCES = test/test_init.c 
#test_app_hint_SOURCES = test/test_app_hint.c
