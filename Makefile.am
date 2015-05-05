AUTOMAKE_OPTIONS = foreign subdir-objects
VERSION_INFO = 1:0:0

#include pwr/Makefile.am
#include plugins/Makefile.am
#include tools/Makefile.am
#include examples/Makefile.am

DIST_SUBDIRS = pwr plugins tools examples

SUBDIRS = pwr plugins tools examples

EXTRA_DIST = LICENSE Changes pwr.m4 autogen.sh  

include_HEADERS = pwr.h

lib_LTLIBRARIES = libpwr.la

libpwr_la_SOURCES = src/pwr.cc src/init.cc src/app_hint.c
libpwr_la_LDFLAGS = -version-info $(VERSION_INFO)

ACLOCAL_AMFLAGS = -I m4

export SHARED=y
export DEBUG=n

SWIG=
DAEMON=

DIRS = pwr plugins tools examples 

if BUILD_SWIG
DIRS += swig
endif

if BUILD_DAEMON
DIRS += daemon 
endif

all: $(DIRS:=Dir)

clean: $(DIRS:=DirClean) 

pwrDir :
	cd $(subst Dir,,$@) ; ${MAKE}

pluginsDir : 
	cd $(subst Dir,,$@) ; ${MAKE}

toolsDir :
	cd $(subst Dir,,$@) ; ${MAKE}

examplesDir :
	cd $(subst Dir,,$@) ; ${MAKE}

swigDir :
	cd $(subst Dir,,$@) ; ${MAKE}

daemonDir :
	cd $(subst Dir,,$@) ; ${MAKE}

pwrDirClean :
	cd $(subst DirClean,,$@) ; ${MAKE} clean

pluginsDirClean : 
	cd $(subst DirClean,,$@) ; ${MAKE} clean

toolsDirClean :
	cd $(subst DirClean,,$@) ; ${MAKE} clean

examplesDirClean :
	cd $(subst DirClean,,$@) ; ${MAKE} clean

swigDirClean :
	cd $(subst DirClean,,$@) ; ${MAKE} clean

daemonDirClean :
	cd $(subst DirClean,,$@) ; ${MAKE} clean
