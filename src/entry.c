#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif
#include <stdio.h>

#include <newt.h>

#include "utils.h"

#define GETCURSORPOSITION  1727051917995085400ull       /* newtEntryGetCursorPosition */
#define GETVALUE           7572409595898050ull          /* newtEntryGetValue */
#define SET                193505681ull                 /* newtEntrySet */
#define SETCOLORS          249906392841078979ull        /* newtEntrySetColors */
#define SETCURSORPOSITION  11163334996565403236ull      /* newtEntrySetCursorPosition */
#define SETFILTER          249906392951374647ull        /* newtEntrySetFilter */
#define SETFLAGS           7572920998621918ull          /* newtEntrySetFlags */

int entry_GetCursorPosition(WORD_LIST *);
int entry_GetValue(WORD_LIST *, char * );
int entry_Set(WORD_LIST *);
int entry_SetColors(WORD_LIST *);
int entry_SetCursorPosition(WORD_LIST *);
int entry_SetFilter(WORD_LIST *);
int entry_SetFlags(WORD_LIST *);
int entry(WORD_LIST* list, char *vname);

static const char * ENTRY_USAGE =		\
  "argument missing\n"				\
  "usage:\n"					\
  "\tnewt entry -v var left top text width flags" \
  "\tnewt entry getvalue -v var entry"; 



int libnewt_entry(WORD_LIST * list) {
  NOT_NULL(list, ENTRY_USAGE);
  /* check for -v */

  char* vname = NULL;
  if (!check_for_v(&list, &vname)) {
     builtin_error( "%s", _(ENTRY_USAGE));	\
    return EX_USAGE;					\
  }

  NOT_NULL(list, ENTRY_USAGE);
  
  /*   lowercase the word */
  lower(&list->word->word);

  switch (djb2_hash(list->word->word)) {

    case GETCURSORPOSITION:
      return entry_GetCursorPosition(list->next);
      break;
    case GETVALUE:
      return entry_GetValue(list->next, vname);
      break;
    case SET:
      return entry_Set(list->next);
      break;
    case SETCOLORS:
      return entry_SetColors(list->next);
      break;
    case SETCURSORPOSITION:
      return entry_SetCursorPosition(list->next);
      break;
    case SETFILTER:
      return entry_SetFilter(list->next);
      break;
    case SETFLAGS:
      return entry_SetFlags(list->next);
      break;
    default:
      NOT_NULL(vname, ENTRY_USAGE);
      return entry(list, vname);
  } 
}



int entry(WORD_LIST* list, char *vname) {
  newtComponent entry;
  char sentry[30];

  int left, top, width, flags;
  char *text = "";
  NOT_NULL(list, ENTRY_USAGE);
  left = (int) strtol(list->word->word, NULL, 10);
  NEXT(list, ENTRY_USAGE);
  top = (int) strtol(list->word->word, NULL, 10);
  NEXT(list, ENTRY_USAGE);
  text = list->word->word;
  NEXT(list, ENTRY_USAGE);
  width = (int) strtol(list->word->word, NULL, 10);    
  NEXT(list, ENTRY_USAGE);
  flags = (int) strtol(list->word->word, NULL, 10);    

  /* no magic result string for now */
  entry = newtEntry(left, top, text, width, NULL, flags);

  snprintf(sentry, 30, "%p", entry);
  
  newt_bind_variable (vname, sentry, 0);
  stupidly_hack_special_variables (vname);

  return 0;
}



int entry_GetCursorPosition(WORD_LIST * list){
   return 0;
}

int entry_GetValue(WORD_LIST * list, char *vname){
  NOT_NULL(list, ENTRY_USAGE);

  newtComponent entry;
  READ_COMPONENT(list->word->word, entry, "Invalid component");
  char * value = newtEntryGetValue(entry);

  newt_bind_variable (vname, value, 0);
  stupidly_hack_special_variables (vname);
  
  return 0;
}

int entry_Set(WORD_LIST * list){
   return 0;
}

int entry_SetColors(WORD_LIST * list){
   return 0;
}

int entry_SetCursorPosition(WORD_LIST * list){
   return 0;
}

int entry_SetFilter(WORD_LIST * list){
   return 0;
}

int entry_SetFlags(WORD_LIST * list){
   return 0;
}
