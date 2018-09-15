
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = newt.la
newt_la_SOURCES = src/bash_to_c.c src/bash_newt.c src/dispatch.c src/newt.c
# this is required to build .so with a nonstandard name, also avoid
# the numbering
newt_la_LDFLAGS= -module -avoid-version

AM_TESTS_ENVIRONMENT = \
	LD_LIBRARY_PATH=$(top_srcdir)/.libs

check_PROGRAMS = test/dispatch test/bash_to_c
TESTS = test/dispatch test/bash_to_c

test_dispatch_CPPFLAGS =  $(BASH_CPPFLAGS) -I$(top_srcdir)/src
test_dispatch_LDFLAGS =  -L$(top_srcdir)/.libs $(BASH_LDFLAGS) 
test_dispatch_LDADD =   -l:newt.so -lcmocka $(BASH_LIBS)
test_bash_to_c_CPPFLAGS =  $(BASH_CPPFLAGS) -I$(top_srcdir)/src
test_bash_to_c_LDFLAGS =  -L$(top_srcdir)/.libs $(BASH_LDFLAGS) 
test_bash_to_c_LDADD =   -l:newt.so -lcmocka $(BASH_LIBS)

#python unittest
pysources = bash_builtin_gen/wrapper_generator.py \
	bash_builtin_gen/testWrapperGenerator.py \
	bash_builtin_gen/__init__.py \
	bash_builtin_gen/__main__.py

pythontests:  $(pysources)
	python -m unittest discover -s $(top_srcdir)

src/bash_newt.h: src/bash_newt_h.tpl pythontests
	python -m bash_builtin_gen  bash_newt_h.tpl pre_processed_newt.h > src/bash_newt.h

src/bash_newt.c: src/bash_newt_c.tpl src/bash_newt.h 
	python -m bash_builtin_gen  bash_newt_c.tpl pre_processed_newt.h > src/bash_newt.c

# just a convenience target
template: src/bash_newt.c src/bash_newt.h

