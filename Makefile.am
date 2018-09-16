
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = newt.la
newt_la_SOURCES = src/bash_newt.c src/type_conversion.c  src/dispatch.c src/newt.c
# this is required to build .so with a nonstandard name, also avoid
# the numbering
newt_la_LDFLAGS= -module -avoid-version 
newt_la_LADD= -lnewt 

AM_TESTS_ENVIRONMENT = \
	LD_LIBRARY_PATH=$(top_srcdir)/.libs

check_PROGRAMS = test/dispatch test/type_conversion test/bash_newt
TESTS = test/dispatch test/type_conversion test/bash_newt


test_cppflags = $(BASH_CPPFLAGS) -I$(top_srcdir)/src
test_ldflags =  -L$(top_srcdir)/.libs $(BASH_LDFLAGS)
test_ldadd =   -l:newt.a -lcmocka $(BASH_LIBS)

test_dispatch_CPPFLAGS =  $(test_cppflags)
test_dispatch_LDFLAGS =  $(test_ldflags)
test_dispatch_LDADD =   $(test_ldadd)

test_type_conversion_CPPFLAGS =  $(test_cppflags)
test_type_conversion_LDFLAGS =   $(test_ldflags)
test_type_conversion_LDADD =    $(test_ldadd)

test_bash_newt_CPPFLAGS =  $(test_cppflags) 
test_bash_newt_LDFLAGS =   -Wl,--wrap=newtInit $(test_ldflags) 
test_bash_newt_LDADD =    $(test_ldadd) 



#python unittest
pysources = bash_builtin_gen/wrapper_generator.py \
	bash_builtin_gen/test_wrapper_generator.py \
	bash_builtin_gen/__init__.py \
	bash_builtin_gen/__main__.py

pythontests:  $(pysources)
	python -m unittest discover -s $(top_srcdir)

test: pythontests check

src/bash_newt.h: src/bash_newt_h.tpl 
	python -m bash_builtin_gen pre_processed_newt.h $< > $@

src/bash_newt.c: src/bash_newt_c.tpl src/bash_newt.h 
	python -m bash_builtin_gen pre_processed_newt.h $<  > $@

clean-local:
	rm -f src/bash_newt.c src/bash_newt.h

# just a convenience target
template: src/bash_newt.c src/bash_newt.h

