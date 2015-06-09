
if HAVE_SWIG

SWIG_SOURCE = pypwr.i
SWIG_OUT = pwr_swig.cpp

CLEANFILES = pwr.py  $(SWIG_OUT)

lib_LTLIBRARIES = _pwr.la

python_PYTHON = pwr.py

_pwr_la_SOURCES = $(SWIG_OUT) 
_pwr_la_CPPFLAGS = $(SWIG_PYTHON_CPPFLAGS) -I$(top_srcdir)/src/pwr
_pwr_la_LDFLAGS = -module -version-info 1:0:0 $(PYTHON_LDFLAGS) -ldl
_pwr_la_LIBADD = $(top_builddir)/src/pwr/libpwr.la 

$(SWIG_OUT) : $(SWIG_SOURCE)
	$(SWIG) $(SWIG_FLAGS) -classic $(PYTHON_CPPFLAGS) \
		-I$(top_srcdir)/src/pwr -o $@ $<

endif

