#ifndef __UTILS_H_
#define __UTILS_H_

#include "bash_includes.h"

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

#endif
