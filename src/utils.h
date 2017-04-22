#ifndef __UTILS_H_
#define __UTILS_H_

#include "bash_includes.h"



unsigned long long djb2_hash(unsigned char *str);

WORD_LIST * next(WORD_LIST ** plist);

// just a macros to advance conditionnally in the list
#define NEXT(L, M)           \
  do {                       \
  if (!next(&L)) {           \
      builtin_error( "%s",  _(M));		\
      return EX_USAGE;            \
 }                           \
} while(0) 

// continue if not null
#define NOT_NULL(L, M)           \
  do {                       \
  if (L == NULL) {           \
      builtin_error( "%s",  _(M));		\
      return EX_USAGE;            \
 }                           \
} while(0) 

/* 
 Checking for 0x at the beginning makes it non portable
 but it's so easy to pass something that crashes bash and not
 so easy to spot the error (eg passing "button" instead of "$button"
 that I'll leave it there until someones wants more portability
 */
#define READ_COMPONENT(S, C, M)			\
  do {                       \
  if (S[0] != '0' || S[1] != 'x'|| sscanf(S, "%p", &C) != 1)  {			\
      builtin_error( "%s",  _(M));		\
      return EX_USAGE;            \
 }                           \
} while(0) 


char* lower(char **ps);

/* borrowed from printf.def */
SHELL_VAR * newt_bind_variable(char *name, char *value, int flags);

/* returns
   1  if the list starts with -v and a valid varname and advance the list
   1  if the list doesn't start with -v, sets vname to NULL
   0  if the list starst with -v and an invalid varname
*/
   
int check_for_v(WORD_LIST** plist, char **pvname);


#endif
