examples_PROGRAMS = dummyTest

# Power API Examples
dummyTest_SOURCES = dummyTest.c
dummyTest_CFLAGS = -I$(top_srcdir)/src/pwr
dummyTest_LDADD = $(top_builddir)/src/pwr/libpwr.la

examplesdir = $(prefix)/examples
examples_configdir = $(examplesdir)/config
examples_scriptdir = $(examplesdir)/script

examples_DATA = dummyTest.c dummyTest.py

examples_config_DATA =	config/cpu-node.xml \
			config/compliance.xml \
			config/dummySystem.xml \
			config/dummySystemLocal.xml \
			config/pg-node.xml \
			config/piapi-node.xml \
			config/piapi-platform.xml \
			config/pi-node.xml \
			config/pi-platform.xml \
			config/rapl-node.xml \
			config/apm-node.xml \
			config/wu-node.xml \
			config/xtpm-node.xml \
			config/xtpm-platform.xml

examples_script_DATA =	script/run_dummy_client.sh \
			script/run_dummy_python.sh \
			script/xml_profile

EXTRA_DIST = $(examples_DATA) $(examples_config_DATA) $(examples_script_DATA)
