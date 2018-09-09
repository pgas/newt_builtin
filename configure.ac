AC_INIT(newt, 0.1)

# Many autoconf project creates a config.h.  however the bash headers
# already use their own version of config.h that, since bash 4.4, is
# installed in include/bash/. As a result you shoud NOT include the
# classic AC_CONFIG_HEADERS([config.h]) choose another name
# if needed ... (the provided config.h already has a a lot)

AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror no-define])
AM_PROG_AR
# we only want to build a shared library
LT_INIT([dlopen disable-static])

# we don't need ldconfig after installing the .so
AC_SUBST([finish_cmds], [""])

#install the .so in $(libdir)/bash
AC_SUBST([libdir], ["$libdir/bash"])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([src/newt.c])

AC_PROG_CC
AC_PROG_INSTALL

# an option to tell configure where the bash sources are
# compiled bash is used to link the unittests
AC_ARG_WITH([bash-sources],
              [AC_HELP_STRING([--with-bash-sources],
              [The prefix where the compiled bash sources resides (default is ../bash)])],
              [with_bash_sources=$withval],
              [with_bash_sources="../bash"])

if test -e $with_bash_sources/hashlib.h
   then
   BASH_CFLAGS="-I$with_bash_sources -I$with_bash_sources/include -I$with_bash_sources/builtins"
   BASH_LDFLAGS="-L./ -L$with_bash_sources/builtins -L$with_bash_sources/lib/readline -L$with_bash_sources/lib/readline -L$with_bash_sources/lib/glob -L$with_bash_sources/lib/tilde -L$with_bash_sources/lib/malloc -L$with_bash_sources/lib/sh -L$with_bash_sources/lib/termcap -rdynamic"
   BASH_LIBS="-lbash -lbuiltins -lglob -lsh -lreadline -lhistory $with_bash_sources/lib/termcap/libtermcap.a -ltilde -lmalloc -ldl" 
   # until I figure a better way to do that
   # I assume ar, running test in the same dir as
   # we don't want main in our lib
   objcopy --redefine-sym main=shell_main $with_bash_sources/shell.o
   ar rvs libbash.a $with_bash_sources/*.o
   OCPPFLAGS=$CPPFLAGS
   CPPFLAGS="$CPPFLAGS $BASH_CPPFLAGS"
   OLDFLAGS=$LDFLAGS
   LDFLAGS="$LDFLAGS $BASH_LDFLAGS"
   OLIBS=$LIBS
   LIBS="$LIBS $BASH_LIBS "
   AC_CHECK_LIB([bash],[hash_create],
      [sourcefound=yes],[CPPFLAGS=$OCPP LDFLAGS=$OLDFLAGS])
fi

if test x"$sourcefound" = xyes
then
   bash_headers=$with_bash_sources
else
   bash_headers=/usr/include
fi

# an option to tell configure where the bash headers are
AC_ARG_WITH([bash-headers],
              [AC_HELP_STRING([--with-bash-headers],
              [The prefix where the bash headers are installed (default is /usr/include)])],
              [with_bash_headers=$withval],
              [with_bash_headers="$bash_headers"])

# try with the bash subdir created when installing headers (bash>=4.4)
# or without if someone tries to link against the sources directly
if test -e $with_bash_headers/bash/builtins.h
  then
   CPPFLAGS="$CPPFLAGS -I$with_bash_headers/bash -I$with_bash_headers/bash/include -I$with_bash_headers/bash/builtins"
  else
CPPFLAGS="$CPPFLAGS -I$with_bash_headers -I$with_bash_headers/include -I$with_bash_headers/builtins"
fi

AC_CHECK_HEADERS([builtins.h],[bashfound=yes], [])
if test x"$bashfound" = x
 then
	AC_MSG_ERROR([unable to find the bash headers please use --with-bash-headers=/path/to/headers/])
fi		     

AC_CHECK_HEADERS([newt.h],[newtfound=yes],[])
if test x"$newtfound" = x
 then
	AC_MSG_ERROR([unable to find the newt headers])
fi		     

AC_CHECK_LIB([newt],[newtInit],[])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT


