#include <stdio.h>

#include "bash_newt.h" 
#include "dispatch.h"

static HASH_TABLE * dispatch_table;


int run(WORD_LIST *list) {
  dispatch_table_run(list->word->word, NULL, dispatch_table); 
}


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

  int exit = run(list);

  if (exit != EX_USAGE)
    {
      return exit;
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
  dispatch_table = dispatch_table_create(entries_length,
  					  entries);
  printf ("newt builtin loaded\n");
  fflush (stdout);
  return (1);
}

void
newt_builtin_unload (s)
     char *s;
{
  dispatch_table_dispose(dispatch_table);
  printf ("newt builtin unloaded\n");
  fflush (stdout);
}
