
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = newt.la
newt_la_SOURCES = src/bash_to_c.c src/bash_newt.c src/dispatch.c src/newt.c
# this is required to build .so with a nonstandard name, also avoid
# the numbering
newt_la_LDFLAGS= -module -avoid-version

check_PROGRAMS = test/dispatch test/bash_to_c
TESTS = test/dispatch test/bash_to_c

test_dispatch_CPPFLAGS =  $(BASH_CPPFLAGS) -I$(top_srcdir)/src
test_dispatch_LDFLAGS =  -L$(top_srcdir)/.libs $(BASH_LDFLAGS) 
test_dispatch_LDADD =   -l:newt.so -lcmocka $(BASH_LIBS)
test_bash_to_c_CPPFLAGS =  $(BASH_CPPFLAGS) -I$(top_srcdir)/src
test_bash_to_c_LDFLAGS =  -L$(top_srcdir)/.libs $(BASH_LDFLAGS) 
test_bash_to_c_LDADD =   -l:newt.so -lcmocka $(BASH_LIBS)


