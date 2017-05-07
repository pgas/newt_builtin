#ifndef __UTILS_H_
#define __UTILS_H_

#include <errno.h>

#include "bash_includes.h"

/**********************
 *  various
 **********************/

/* compute hash from a string */
unsigned long long djb2_hash(unsigned char *str);
/* lowercase the string  in place */
char* lower(char **ps);

/**********************
 *  list manipulation
 **********************/
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



/**********************************
 * 
 * transform strings into C arguments 
 *
 **********************/



/* 
 Checking for 0x at the beginning makes it non portable
 but it's so easy to pass something that crashes bash and not
 so easy to spot the error (eg passing "button" instead of "$button"
 that I'll leave it there until someones wants more portability
 */
#define READ_COMPONENT(L, C, M)			\
  NEXT(L, M); \
  bash_component C; \
  do {                       \
  if (L->word->word[0] != '0' || L->word->word[1] != 'x'|| sscanf(L->word->word, "%p", &C) != 1)  {			\
      builtin_error( "%s",  _(M));		\
      return EX_USAGE;            \
 }                           \
} while(0) 

#define READ_INT(L, I, M) \
  int I; \
  do {                       \
  NEXT(L, M);						\
  I = (int) strtol(L->word->word, NULL, 10);	\
} while(0) 

#define READ_INT_OPT(L, I, D, M)		\
  int I = D; \
  do { \
  if (next(&L)) { \
      I = (int) strtol(L->word->word, NULL, 10); \
  } \
} while(0) 


#define READ_STRING(L, S, M) \
  char * S; \
  do {                       \
  NEXT(L, M);						\
  S = L->word->word;	\
} while(0) 


#define WRITE_POINTER(V, F) \
  do { \
  char pointer_string[30]; \
  void * pointer = (void *) F; \
  snprintf(pointer_string, 30, "%p", pointer); \
  newt_bind_variable (V, pointer_string, 0); \
  stupidly_hack_special_variables (V); \
} while(0)

#define WRITE_INT(V, F) \
  do { \
  char value_string[30]; \
  int value  = (int) F;			       \
  snprintf(value_string, 30, "%d", value); \
  newt_bind_variable (V, value_string, 0); \
  stupidly_hack_special_variables (V); \
} while(0)

#define WRITE_STRING(V, F) \
  do { \
  char * value = F;			       \
  newt_bind_variable (V, value, 0); \
  stupidly_hack_special_variables (V); \
} while(0)

#define WRITE_CHAR(V, F) \
  do { \
  char * ch = F;			       \
  char value[2] = { ch,  0 }; \
  newt_bind_variable (V, value, 0); \
  stupidly_hack_special_variables (V); \
} while(0)




/**********************
 *  Shell Variable
 **********************/
/* borrowed from printf.def */
SHELL_VAR * newt_bind_variable(char *name, char *value, int flags);
int valid_variable(char* v);

/* returns
   1  if the list starts with -v and a valid varname and advance the list
   1  if the list doesn't start with -v, sets vname to NULL
   0  if the list starst with -v and an invalid varname
*/

/* deprecated */
int check_for_v(WORD_LIST** plist, char **pvname);



#endif
