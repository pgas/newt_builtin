#ifndef __UTILS_H_
#define __UTILS_H_

#include "bash_includes.h"

unsigned long long djb2_hash(unsigned char *str);

int next(WORD_LIST ** plist);

// just a macros to advance conditionnally in the list
#define NEXT(L, M)           \
  do {                       \
  if (!next(&L)) {           \
      fprintf(stderr, "%s",  M);    \
      return 127;            \
 }                           \
} while(0) 

// continue if not null
#define NOT_NULL(L, M)           \
  do {                       \
  if (L == NULL) {           \
      fprintf(stderr, "%s",  M);    \
      return 127;            \
 }                           \
} while(0) 


void lower(char **ps);

/* borrowed from printf.def */
SHELL_VAR * newt_bind_variable(char *name, char *value, int flags);

/* returns
   1  if the list starts with -v and a valid varname and advance the list
   1  if the list doesn't start with -v, sets vname to NULL
   0  if the list starst with -v and an invalid varname
*/
   
int check_for_v(WORD_LIST** plist, char **pvname);


#endif
