examples_PROGRAMS = snippet dummyTest

# Power API Examples
snippet_SOURCES = snippet.c
snippet_CFLAGS = -I$(top_srcdir)/pwr
snippet_LDADD = $(top_builddir)/pwr/libpwr.la
dummyTest_SOURCES = dummyTest.c
dummyTest_CFLAGS = -I$(top_srcdir)/pwr
dummyTest_LDADD = $(top_builddir)/pwr/libpwr.la

examplesdir = $(docdir)/examples
examples_configdir = $(examplesdir)/config
examples_scriptdir = $(examplesdir)/script
examples_DATA = snippet.c snippet.py dummyTest.c dummyTest.py
examples_config_DATA =	config/cpu-node.xml \
			config/dummySystem.xml \
			config/pg-node.xml \
			config/pi-node.xml \
			config/pi-platform.xml \
			config/rapl-node.xml \
			config/wu-node.xml \
			config/xtpm-node.xml \
			config/xtpm-platform.xml
examples_script_DATA =	script/run_dummy_client.sh \
			script/run_dummy_python.sh \
			script/run_dummy.sh \
			script/run-snippet-python.sh \
			script/run-snippet.sh

EXTRA_DIST = $(examples_DATA) $(examples_config_DATA) $(examples_script_DATA)

