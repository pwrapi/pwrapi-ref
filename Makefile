
all: powDir pluginsDir toolsDir examplesDir swigDir

clean: powDirClean pluginsDirClean toolsDirClean  examplesDirClean swigDirClean


powDir :
	cd pow ; ${MAKE} 

pluginsDir : 
	cd plugins ; ${MAKE}

toolsDir :
	cd tools; ${MAKE}

examplesDir :
	cd examples; ${MAKE}

swigDir :
	cd swig; ${MAKE}

powDirClean :
	cd pow ; ${MAKE} clean

pluginsDirClean : 
	cd plugins ; ${MAKE} clean

toolsDirClean :
	cd tools; ${MAKE} clean

examplesDirClean :
	cd examples; ${MAKE} clean

swigDirClean :
	cd swig; ${MAKE} clean

