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
  "\tnewt form Run|AddHotKey|GetScrollPosition|SetScrollPosition|Destroy\n";



int libnewt_form(WORD_LIST * list) {
  /* check for -v */
  char* vname;
  if (!check_for_v(&list, &vname)) {
    fprintf(stderr, "%s", FORM_USAGE);			\
    return 127;					\
  }

  /*   lowercase the word */
  lower(&list->word->word);
  switch (djb2_hash(list->word->word)) {
    case ADDCOMPONENT:
      return form_AddComponent(list->next);
      break;
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


  newt_bind_variable (vname, "", 0);
  stupidly_hack_special_variables (vname);

}

int form_AddComponent(WORD_LIST * list){
   return 0;
}

int form_AddComponents(WORD_LIST * list){
   return 0;
}

int form_AddHotKey(WORD_LIST * list){
   return 0;
}

int form_Destroy(WORD_LIST * list){
   return 0;
}

int form_GetCurrent(WORD_LIST * list){
   return 0;
}

int form_GetScrollPosition(WORD_LIST * list){
   return 0;
}

int form_Run(WORD_LIST * list){
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
