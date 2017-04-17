/* Sample builtin to be dynamically loaded with enable -f and create a new
   builtin. */

/* Except for the included headers from "loadables.h" that have been
   inline manually (loadables.h is part of the examples and is not
   installe by bash, this is the same builtin example from the
   loadables examples that is found in the bash sources
 */

/*
   Copyright (C) 1999-2009 Free Software Foundation, Inc.

   This file is part of GNU Bash.
   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/



#include <config.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <stdio.h>

#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"
#include "common.h"

#include <newt.h>

/* A builtin `xxx' is normally implemented with an `xxx_builtin' function.
   If you're converting a command that uses the normal Unix argc/argv
   calling convention, use argv = make_builtin_argv (list, &argc) and call
   the original `main' something like `xxx_main'.  Look at cat.c for an
   example.

   Builtins should use internal_getopt to parse options.  It is the same as
   getopt(3), but it takes a WORD_LIST *.  Look at print.c for an example
   of its use.

   If the builtin takes no options, call no_options(list) before doing
   anything else.  If it returns a non-zero value, your builtin should
   immediately return EX_USAGE.  Look at logname.c for an example.

   A builtin command returns EXECUTION_SUCCESS for success and
   EXECUTION_FAILURE to indicate failure. */
int
newt_builtin (list)
     WORD_LIST *list;
{
  int c;
  reset_internal_getopt ();
  while ((c = internal_getopt (list, "")) != -1) {
    switch (c)
      {
      case GETOPT_HELP:
	builtin_help (); 
	return (EX_USAGE);
      default:
	builtin_usage ();
	return (EX_USAGE);
      }
  }
  list = loptend;
  if (list == NULL)
    {
      builtin_usage ();
      return (EX_USAGE); 
    }
    begin_unwind_frame ("newt_builtin_env");
  if (temporary_env)
    {
      push_scope (VC_BLTNENV, temporary_env);
      temporary_env = (HASH_TABLE *)NULL;	  
      add_unwind_protect (pop_scope, ( CMD_COMMAND_BUILTIN) ? 0 : "1");
    }

  if (strncmp(list->word->word, "init", 4) == 0 )
    {
      return newtInit();
    }
  else
    {
       builtin_usage ();
      return (EX_USAGE);
    }
  if (temporary_env)
    run_unwind_frame ("newt_builtin_env");
  return (EXECUTION_SUCCESS);
}

int
newt_builtin_load (s)
     char *s;
{
  printf ("newt builtin loaded\n");
  fflush (stdout);
  return (1);
}

void
newt_builtin_unload (s)
     char *s;
{
  printf ("newt builtin unloaded\n");
  fflush (stdout);
}

/* An array of strings forming the `long' documentation for a builtin xxx,
   which is printed by `help xxx'.  It must end with a NULL.  By convention,
   the first line is a short description. */
char *newt_doc[] = {
	"Sample builtin.",
	"",
	"this is the long doc for the sample newt builtin",
	(char *)NULL
};

/* The standard structure describing a builtin command.  bash keeps an array
   of these structures.  The flags must include BUILTIN_ENABLED so the
   builtin can be used. */
struct builtin newt_struct = {
	"newt",		/* builtin name */
	newt_builtin,		/* function implementing the builtin */
	BUILTIN_ENABLED,	/* initial flags for builtin */
	newt_doc,		/* array of long documentation strings. */
	"newt",		/* usage synopsis; becomes short_doc */
	0			/* reserved for internal use */
};
