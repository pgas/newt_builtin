#include "utils.h"

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
