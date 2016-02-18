examples_PROGRAMS = snippet dummyTest simpleTest

# Power API Examples
snippet_SOURCES = snippet.c
snippet_CFLAGS = -I$(top_srcdir)/src/pwr
snippet_LDADD = $(top_builddir)/src/pwr/libpwr.la

dummyTest_SOURCES = dummyTest.c
dummyTest_CFLAGS = -I$(top_srcdir)/src/pwr
dummyTest_LDADD = $(top_builddir)/src/pwr/libpwr.la

simpleTest_SOURCES = simpleTest.cc
simpleTest_CPPFLAGS = -I$(top_srcdir)/src/pwr
simpleTest_LDADD = $(top_builddir)/src/pwr/libpwr.la

examplesdir = $(docdir)/examples
examples_configdir = $(examplesdir)/config
examples_scriptdir = $(examplesdir)/script
examples_DATA = snippet.c snippet.py dummyTest.c dummyTest.py simpleTest.cc
examples_config_DATA =	config/cpu-node.xml \
			config/dummySystem.xml \
			config/pg-node.xml \
			config/piapi-node.xml \
			config/piapi-platform.xml \
			config/pi-node.xml \
			config/pi-platform.xml \
			config/rapl-node.xml \
			config/opt-node.xml \
			config/wu-node.xml \
			config/xtpm-node.xml \
			config/xtpm-platform.xml
examples_script_DATA =	script/run_dummy_client.sh \
			script/run_dummy_python.sh \
			script/run_dummy.sh \
			script/run-snippet-python.sh \
			script/run-snippet.sh

EXTRA_DIST = $(examples_DATA) $(examples_config_DATA) $(examples_script_DATA)

