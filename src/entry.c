#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif
#include <stdio.h>

#include <newt.h>

#include "bash_includes.h"
#include "utils.h"
#include "libnewt.h"

#define GETCURSORPOSITION  1727051917995085400ull       /* newtEntryGetCursorPosition */
#define GETVALUE           7572409595898050ull          /* newtEntryGetValue */
#define SET                193505681ull                 /* newtEntrySet */
#define SETCOLORS          249906392841078979ull        /* newtEntrySetColors */
#define SETCURSORPOSITION  11163334996565403236ull      /* newtEntrySetCursorPosition */
#define SETFILTER          249906392951374647ull        /* newtEntrySetFilter */
#define SETFLAGS           7572920998621918ull          /* newtEntrySetFlags */

int entry_GetCursorPosition(WORD_LIST *, char *);
int entry_GetValue(WORD_LIST *, char * );
int entry_Set(WORD_LIST *);
int entry_SetColors(WORD_LIST *);
int entry_SetCursorPosition(WORD_LIST *);
int entry_SetFilter(WORD_LIST *);
int entry_SetFlags(WORD_LIST *);
int entry(WORD_LIST* list, char *vname);

int entry_filter(newtComponent entry, void * data, int ch,
		 int cursor) {
  bash_component bash_entry = (bash_component) data;
  char sentry[30];
  snprintf(sentry, 30, "%p", bash_entry);
  newt_bind_variable ("NEWT_ENTRY", sentry, 0);
  char sch[2] = { 0 };
  sch[0]=ch;
  newt_bind_variable ("NEWT_CH", sch, 0);
  char scursor[30];
  snprintf(scursor, 30, "%i", cursor);
  newt_bind_variable ("NEWT_CURSOR", scursor, 0);
  
  int flags =  SEVAL_NOHIST;
  int ret =  evalstring (savestring(bash_entry->filter), NULL, flags);
  return ret==0?ch:0;
}

static const char * ENTRY_USAGE =		\
  "argument missing\n"				\
  "usage:\n"					\
  "\tnewt entry -v var left top text width flags\n" \
  "\tnewt entry getvalue -v var entry"; 

int libnewt_entry(WORD_LIST * list) {  
  char* vname = NULL;
  int c;
  reset_internal_getopt ();
  while ((c = internal_getopt (list, "v:h")) != -1) {
    switch (c)
      {
      case 'v':
	vname = list_optarg;
	if (!valid_variable(vname)) {
	  return (EX_USAGE);
	}
	break;
      case 'h':
	return (EX_USAGE);
      case GETOPT_HELP:
	/* TODO 	builtin_help (); */
	return (EX_USAGE);
      default:
	builtin_usage ();
	return (EX_USAGE);
      }
  }
  list = loptend;

  NOT_NULL(list, ENTRY_USAGE);
  
  /*   lowercase the word */
  lower(&list->word->word);

  switch (djb2_hash(list->word->word)) {

    case GETCURSORPOSITION:
      return entry_GetCursorPosition(list, vname);
      break;
    case GETVALUE:
      return entry_GetValue(list, vname);
      break;
    case SET:
      return entry_Set(list);
      break;
    case SETCOLORS:
      return entry_SetColors(list);
      break;
    case SETCURSORPOSITION:
      return entry_SetCursorPosition(list);
      break;
    case SETFILTER:
      return entry_SetFilter(list);
      break;
    case SETFLAGS:
      return entry_SetFlags(list);
      break;
    default:
      NOT_NULL(vname, ENTRY_USAGE);
      return entry(list, vname);
  } 
}

int entry(WORD_LIST* list, char *vname) {
  /* just so that it works like the other commands */
  WORD_LIST dummy = { list,  NULL };
  list = &dummy;
  
  READ_INT(list, left, ENTRY_USAGE);
  READ_INT(list, top, ENTRY_USAGE);
  READ_STRING(list, text, ENTRY_USAGE);
  READ_INT(list, width, ENTRY_USAGE);
  READ_INT_OPT(list, flags, ENTRY_USAGE);

  WRITE_POINTER(vname, new_bash_component(newtEntry(left, top, text, width, NULL, flags)));

  return 0;
}

int entry_GetCursorPosition(WORD_LIST * list, char *vname){
  NEXT(list, ENTRY_USAGE);
  bash_component entry;
  READ_COMPONENT(list->word->word, entry, "Invalid component");

  int value = newtEntryGetCursorPosition(entry->co);
  fprintf(stderr, "cursor pos %d\n", value);
  char svalue[30];
  snprintf(svalue, 30, "%d", value);
  newt_bind_variable (vname, svalue, 0);
  stupidly_hack_special_variables (vname);
  
  return 0;
}

int entry_GetValue(WORD_LIST * list, char *vname){
  NEXT(list, ENTRY_USAGE);
  bash_component entry;
  READ_COMPONENT(list->word->word, entry, "Invalid component");
  char * value = newtEntryGetValue(entry->co);

  newt_bind_variable (vname, value, 0);
  stupidly_hack_special_variables (vname);
  
  return 0;
}

int entry_Set(WORD_LIST * list){
  NEXT(list, ENTRY_USAGE);
  bash_component entry;
  READ_COMPONENT(list->word->word, entry, "Invalid component");
  NEXT(list, ENTRY_USAGE);
  char * value = list->word->word;
  int cursorAtEnd = 1;
  if (next(&list)) {
    cursorAtEnd = (int) strtol(list->word->word, NULL, 10);
  }
  newtEntrySet(entry->co, value, cursorAtEnd);
  return 0;
}

int entry_SetColors(WORD_LIST * list){
    /* TODO */
   return 0;
}

int entry_SetCursorPosition(WORD_LIST * list){
    /* TODO */
   return 0;
}

int entry_SetFilter(WORD_LIST * list){

  NEXT(list, ENTRY_USAGE);
  bash_component entry;
  READ_COMPONENT(list->word->word, entry, "Invalid component");
  NEXT(list, ENTRY_USAGE);
  entry->filter = savestring(list->word->word);
  newtEntrySetFilter(entry->co, entry_filter, (void *) entry );
  return 0;
}

int entry_SetFlags(WORD_LIST * list){
  /* TODO */
   return 0;
}
