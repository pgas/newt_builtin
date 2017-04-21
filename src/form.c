#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif
#include <stdio.h>

#include <newt.h>

#include "utils.h"

#define ADDCOMPONENT       14856387628357384193ull      /* newtFormAddComponent */
#define ADDCOMPONENTS      10645445819345336468ull      /* newtFormAddComponents */
#define ADDHOTKEY          249881014399979842ull        /* newtFormAddHotKey */
#define DESTROY            229463081808367ull           /* newtFormDestroy */
#define GETCURRENT         8246354026184804360ull       /* newtFormGetCurrent */
#define GETSCROLLPOSITION  3794203164532068073ull       /* newtFormGetScrollPosition */
#define RUN                193505114ull                 /* newtFormRun */
#define SETBACKGROUND      3848949777203120913ull       /* newtFormSetBackground */
#define SETCURRENT         8246910963997627796ull       /* newtFormSetCurrent */
#define SETHEIGHT          249906393024779882ull        /* newtFormSetHeight */
#define SETSCROLLPOSITION  13230486243102385909ull      /* newtFormSetScrollPosition */
#define SETSIZE            229482454968428ull           /* newtFormSetSize */
#define SETTIMER           7572921015130002ull          /* newtFormSetTimer */
#define SETWIDTH           7572921018678449ull          /* newtFormSetWidth */
#define WATCHFD            229487463721990ull           /* newtFormWatchFd */

int form(WORD_LIST *, char *vname);
int form_AddComponent(WORD_LIST *);
int form_AddComponents(WORD_LIST *);
int form_AddHotKey(WORD_LIST *);
int form_Destroy(WORD_LIST *);
int form_GetCurrent(WORD_LIST *);
int form_GetScrollPosition(WORD_LIST *);
int form_Run(WORD_LIST *);
int form_SetBackground(WORD_LIST *);
int form_SetCurrent(WORD_LIST *);
int form_SetHeight(WORD_LIST *);
int form_SetScrollPosition(WORD_LIST *);
int form_SetSize(WORD_LIST *);
int form_SetTimer(WORD_LIST *);
int form_SetWidth(WORD_LIST *);
int form_WatchFd(WORD_LIST *);

static const char * FORM_USAGE =      \
  "argument missing or invalid\n"				\
  "usage:\n"					\
  "\tnewt form -v var [vertBar [helpTag [flags]]]\n"	\
  "\tnewt form SetTimer|WatchFd|SetSize|GetCurrent|SetBackground\n" \
  "\tnewt form SetCurrent|AddComponent|AddComponents|SetHeight|SetWidth\n"\
  "\tnewt form Run|AddHotKey|GetScrollPosition|SetScrollPosition|Destroy";


int libnewt_form(WORD_LIST * list) {
  NOT_NULL(list, FORM_USAGE);
  /* check for -v */

  char* vname = NULL;
  if (!check_for_v(&list, &vname)) {
     builtin_error( "%s", _(FORM_USAGE));	\
    return EX_USAGE;					\
  }

  /*   lowercase the word */
  lower(&list->word->word);
  switch (djb2_hash(list->word->word)) {
    case ADDCOMPONENT:
    case ADDCOMPONENTS:
      return form_AddComponents(list->next);
      break;
    case ADDHOTKEY:
      return form_AddHotKey(list->next);
      break;
    case DESTROY:
      return form_Destroy(list->next);
      break;
    case GETCURRENT:
      return form_GetCurrent(list->next);
      break;
    case GETSCROLLPOSITION:
      return form_GetScrollPosition(list->next);
      break;
    case RUN:
      return form_Run(list->next);
      break;
    case SETBACKGROUND:
      return form_SetBackground(list->next);
      break;
    case SETCURRENT:
      return form_SetCurrent(list->next);
      break;
    case SETHEIGHT:
      return form_SetHeight(list->next);
      break;
    case SETSCROLLPOSITION:
      return form_SetScrollPosition(list->next);
      break;
    case SETSIZE:
      return form_SetSize(list->next);
      break;
    case SETTIMER:
      return form_SetTimer(list->next);
      break;
    case SETWIDTH:
      return form_SetWidth(list->next);
      break;
    case WATCHFD:
      return form_WatchFd(list->next);
      break;
    default:
      NOT_NULL(vname, FORM_USAGE);
      return form(list->next, vname);
  } 
}


int form(WORD_LIST* list, char *vname) {
  newtComponent form;
  char sform[30];
  
  if (list == NULL) {
    form = newtForm(NULL, NULL, 0);
  }
  
  snprintf(sform, 30, "%p", form);
  
  newt_bind_variable (vname, sform, 0);
  stupidly_hack_special_variables (vname);

  return 0;
}

/* int form_AddComponent(WORD_LIST * list){ */
/*    return 0; */
/* } */

static const char * ADDCOMPONENT_USAGE =      \
  "argument missing or invalid\n"				\
  "usage:\n"					\
  "\tnewt form addcomponents form component1 [components]";

int form_AddComponents(WORD_LIST * list){
  NOT_NULL(list, ADDCOMPONENT_USAGE);
  newtComponent form;
  READ_COMPONENT(list->word->word, form, "Invalid component");
  NEXT(list, ADDCOMPONENT_USAGE);
  do {
    newtComponent c;
    READ_COMPONENT(list->word->word, c, "Invalid component");
    newtFormAddComponent(form, c);
  } while (next(&list));
  return 0;
}

int form_AddHotKey(WORD_LIST * list){
   return 0;
}


static const char * FORMDESTROY_USAGE =      \
  "argument missing or invalid\n"				\
  "usage:\n"					\
  "\tnewt form destroy form ";

int form_Destroy(WORD_LIST * list){
  NOT_NULL(list, FORMDESTROY_USAGE);
  newtComponent form;
  READ_COMPONENT(list->word->word, form, _("Invalid component"));
  newtFormDestroy(form);
  return 0;
}

int form_GetCurrent(WORD_LIST * list){
   return 0;
}

int form_GetScrollPosition(WORD_LIST * list){
   return 0;
}

int form_Run(WORD_LIST * list){
  /* TODO: Return value */
  NOT_NULL(list, ADDCOMPONENT_USAGE);
  newtComponent form;
  READ_COMPONENT(list->word->word, form, "Invalid component");
  struct newtExitStruct  es;
  newtFormRun(form, &es);

   return 0;
}

int form_SetBackground(WORD_LIST * list){
   return 0;
}

int form_SetCurrent(WORD_LIST * list){
   return 0;
}

int form_SetHeight(WORD_LIST * list){
   return 0;
}

int form_SetScrollPosition(WORD_LIST * list){
   return 0;
}

int form_SetSize(WORD_LIST * list){
   return 0;
}

int form_SetTimer(WORD_LIST * list){
   return 0;
}

int form_SetWidth(WORD_LIST * list){
   return 0;
}

int form_WatchFd(WORD_LIST * list){
   return 0;
}