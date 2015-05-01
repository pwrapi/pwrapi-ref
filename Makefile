
SWIG=
DAEMON=

DIRS = pow plugins tools examples 

ifeq ($(SWIG),y)
DIRS += swig
endif

ifeq ($(DAEMON),y)
DIRS += daemon 
endif

all: $(DIRS:=Dir)

clean: $(DIRS:=DirClean) 

powDir :
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

powDirClean :
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
