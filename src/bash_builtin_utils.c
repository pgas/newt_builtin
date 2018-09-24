#include "bash_builtin_utils.h"
#include <stdio.h>
#include <stdarg.h>


/* borrowed from printf.def */
SHELL_VAR *bash_builtin_utils_bind_variable(char *name, char *value, int flags){
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

bool bash_builtin_utils_is_a_legal_name(char* name){
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


WORD_LIST * bash_builtin_utils_word(const char* string) {
  WORD_LIST * args = xmalloc(sizeof(WORD_LIST));
  args->word =  xmalloc(sizeof(WORD_DESC));
  args->word->word = savestring(string);
  args->next = NULL;
  return args;
}

void  bash_builtin_utils_free_word(WORD_LIST * args) {
  xfree(args->word->word);
  xfree(args->word);
  xfree(args);
}

WORD_LIST * bash_builtin_utils_make_word_list(const char* arg0, ...){
  va_list args;
  va_start(args, arg0);
  WORD_LIST * list = bash_builtin_utils_word(arg0);
  WORD_LIST * prev = list;
  const char * next_word = va_arg(args, const char*);
  while (next_word != NULL) {
    prev->next = bash_builtin_utils_word(next_word);
    prev = prev->next;
    next_word = va_arg(args, const char*);
  }
  return list;
}

void bash_builtin_utils_free_word_list(WORD_LIST *w){
  while (w != NULL) {
    WORD_LIST *next = w->next;
    bash_builtin_utils_free_word(w);
    w = next;
  }
}

void bash_builtin_utils_print_word_list(WORD_LIST *w){
  while (w != NULL) {
    WORD_LIST *next = w->next;
    fprintf(stderr, "word: %s\n", w->word->word);
    w = next;
  }
}
