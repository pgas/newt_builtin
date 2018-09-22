#include "utils.h"


int compare_ptr (const void *a, const void *b) {
  void ** da = (void **)a;
  void ** db = (void **)b;
  return (*da < *db);
}

/* borrowed from printf.def */
SHELL_VAR * newt_bind_variable(char *name, char *value, int flags){
    SHELL_VAR *v;

#if defined (ARRAY_VARS)
  if (valid_array_reference (name, 0) == 0)
    v = bind_variable (name, value, flags);
  else
    v = assign_array_element (name, value, flags);
#else /* !ARRAY_VARS */
  v = bind_variable (name, value, flags);
#endif /* !ARRAY_VARS */

  if (v && readonly_p (v) == 0 && noassign_p (v) == 0)
    VUNSETATTR (v, att_invisible);

  return v;
}

bool is_a_legal_name(const char* name){
  #if defined (ARRAY_VARS)
    if (legal_identifier (name) || valid_array_reference (name, 0))
#else
      if (legal_identifier (name))
#endif
	{
	  return true;
	} else {
	sh_invalidid (name);
	return false;
      }
}
