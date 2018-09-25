
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = newt.la
newt_la_SOURCES = \
	src/bash_newt.c \
	src/bash_newt_types.c \
	src/type_conversion.c \
	src/dispatch.c \
	src/bash_builtin_utils.c \
	src/newt.c

# this is required to build .so with a nonstandard name, also avoid
# the numbering
newt_la_CPPFLAGS = $(BASH_CPPFLAGS)
newt_la_LDFLAGS= -module -avoid-version 
newt_la_LADD= -lnewt 


if INCLUDE_LIBBASH
# the - before include is a trick? to avoid automake parsing
-include libbash.mk
endif

AM_TESTS_ENVIRONMENT = \
	LD_LIBRARY_PATH=$(top_srcdir)/.libs

check_PROGRAMS = test/bash_newt_types test/dispatch test/bash_builtin_utils test/type_conversion test/bash_newt 
TESTS =  test/bash_newt_types test/dispatch test/bash_builtin_utils test/type_conversion test/bash_newt

test_cppflags = $(BASH_CPPFLAGS) -I$(top_srcdir)/src
test_ldflags =  -L$(top_srcdir)/.libs $(BASH_LDFLAGS)  -Wl,--wrap=main
test_ldadd =   -l:newt.a -lcmocka $(BASH_LIBS)

test_bash_newt_types_CPPFLAGS =  $(test_cppflags)
test_bash_newt_types_LDFLAGS =  $(test_ldflags)
test_bash_newt_types_LDADD =   $(test_ldadd)
EXTRA_test_bash_newt_types_DEPENDENCIES = $(newt_la_SOURCES)


test_bash_builtin_utils_CPPFLAGS =  $(test_cppflags)
test_bash_builtin_utils_LDFLAGS =  $(test_ldflags)
test_bash_builtin_utils_LDADD =   $(test_ldadd)
EXTRA_test_bash_builtin_utils_DEPENDENCIES = $(newt_la_SOURCES)


test_dispatch_CPPFLAGS =  $(test_cppflags)
test_dispatch_LDFLAGS =  $(test_ldflags)
test_dispatch_LDADD =   $(test_ldadd)
EXTRA_test_dispatch_DEPENDENCIES = $(newt_la_SOURCES)

test_type_conversion_CPPFLAGS =  $(test_cppflags)
test_type_conversion_LDFLAGS =   -Wl,--wrap=newtComponentAddDestroyCallback \
				$(test_ldflags)
test_type_conversion_LDADD =    $(test_ldadd)
EXTRA_test_type_conversion_DEPENDENCIES = $(newt_la_SOURCES)

test_bash_newt_CPPFLAGS =  $(test_cppflags) 
test_bash_newt_LDFLAGS =   -Wl,--wrap=newtInit \
			   -Wl,--wrap=newtDrawRootText\
			   -Wl,--wrap=newtCompactButton \
			   -Wl,--wrap=newtComponentAddDestroyCallback \
			   $(test_ldflags) 
test_bash_newt_LDADD =    $(test_ldadd)
EXTRA_test_bash_newt_DEPENDENCIES = $(newt_la_SOURCES)

#python unittest
pysources = bash_builtin_gen/wrapper_generator.py \
	bash_builtin_gen/test_wrapper_generator.py \
	bash_builtin_gen/__init__.py \
	bash_builtin_gen/__main__.py

pythontests:  $(pysources)
	python -m unittest discover -s $(top_srcdir)

test: pythontests check

src/bash_newt.h: templates/bash_newt_h.tpl 
	python -m bash_builtin_gen pre_processed_newt.h $< > $@

src/bash_newt.c: templates/bash_newt_c.tpl src/bash_newt.h 
	python -m bash_builtin_gen pre_processed_newt.h $<  > $@

clean-local:
	rm -f src/bash_newt.c src/bash_newt.h

# just a convenience target
template: src/bash_newt.c src/bash_newt.h

