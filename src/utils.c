#include "utils.h"


/* hashing using djb2*/
unsigned long long djb2_hash(unsigned char *str) {
  unsigned long long hash = 5381;
  int c;
  while (c = *str++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}


int next(WORD_LIST ** plist) {
  if ((*plist)->next != NULL){
    *plist=((*plist)->next);
    return 1;
  } else {
    return 0;
  }
}

void lower(char **ps) {
  char *s = *ps;
  for ( ; *s; ++s) *s = tolower(*s);
}

/* borrowed from printf.def */
SHELL_VAR *
newt_bind_variable (name, value, flags)
     char *name;
     char *value;
     int flags;
{
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

/* returns
   1  if the list starts with -v and a valid varname and advance the list
   1  if the list doesn't start with -v, sets vname to NULL
   0  if the list starst with -v and an invalid varname
*/
   
int check_for_v(WORD_LIST** plist, char **pvname) {
  char* w = (*plist)->word->word;
  if ((w[0] == '-') && (w[1] == 'v')) {
    if (w[0] = '\0') {
      if (next(plist)) {
	w = (*plist)->word->word;
      } else {
	return 0;
      }
    } else {
      w = &w[2];
    }
#if defined (ARRAY_VARS)
    if (legal_identifier (w) || valid_array_reference (w, 0)) 
#else
      if (legal_identifier (w)) 
#endif
	{
	  pvname = &w;
	  return 1;
	} else {
	sh_invalidid (*pvname);
	return 0;
      }
  }
  pvname = NULL;
  return 1;
}
