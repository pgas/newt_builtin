#include "libnewt.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <stdio.h>


#include <newt.h>

#define BELL 6385076388ull
#define BUTTON 6953367461249ull
#define CENTEREDWINDOW 2410708038688732711ull
#define CHECKBOX 7572242387314956ull
#define CLEARKEYBUFFER 13602504209690105039ull
#define CLS 193488487ull
#define COMPACTBUTTON 14551555468575447720ull
#define COMPONENT 249884307956162072ull
#define CREATEGRID 8246186021643569407ull
#define CURSOR 6953406523683ull
#define DELAY 210709892404ull
#define DRAW 6385162067ull
#define ENTRY 210711411031ull
#define FINISHED 7572371893739599ull
#define FORM 6385231225ull
#define GETSCREENSIZE 2280326943471863648ull
#define GRID 6385270123ull
#define INIT 6385337657ull
#define LABEL 210719225253ull
#define LISTBOX 229473570076266ull
#define LISTITEM 7572627812773264ull
#define POP 193502740ull
#define PUSHHELPLINE 15641257600514309558ull
#define RADIO 210726343092ull
#define REDRAWHELPLINE 15850614626379253499ull
#define REFRESH 229481146857044ull
#define RESIZESCREEN 15717840086115858263ull
#define RESUME 6953974617878ull
#define RUNFORM 229481780940334ull
#define SCALE 210727597709ull
#define SCROLLBAR 249906305487309321ull
#define SET 193505681ull
#define SUSPEND 229483079836231ull
#define TEXTBOX 229483751208051ull
#define WAITFORKEY 8247090535609895530ull
#define WINMESSAGE 8247101950821806360ull


/* hashing using djb2...gives unique hashes for the above commands*/
unsigned long long hash(unsigned char *str) {
  unsigned long long hash = 5381;
  int c;
  while (c = *str++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

/* forward declarations */
int libnewt_cls(WORD_LIST *);
int libnewt_draw(WORD_LIST *);
int libnewt_finished(WORD_LIST *);
int libnewt_init(WORD_LIST *);
int libnewt_pop(WORD_LIST *);
int libnewt_pushHelpLine(WORD_LIST *);
int libnewt_refresh(WORD_LIST *);


/* 
  parse the commands and execute accordingly
  returns 127 in case of unknown command,
  
*/ 
  
int libnewt_run(WORD_LIST * list) {
  switch (hash(list->word->word)) {
  /* case BELL: */
  /*   return libnewt_bell(list->next); */
  /*   break; */
  /* case BUTTON: */
  /*   return libnewt_button(list->next); */
  /*   break; */
  /* case CENTEREDWINDOW: */
  /*   return libnewt_centeredWindow(list->next); */
  /*   break; */
  /* case CHECKBOX: */
  /*   return libnewt_checkbox(list->next); */
  /*   break; */
  /* case CLEARKEYBUFFER: */
  /*   return libnewt_clearKeyBuffer(list->next); */
  /*   break; */
  case CLS:
    return libnewt_cls(list->next);
    break;
  /* case COMPACTBUTTON: */
  /*   return libnewt_compactButton(list->next); */
  /*   break; */
  /* case COMPONENT: */
  /*   return libnewt_component(list->next); */
  /*   break; */
  /* case CREATEGRID: */
  /*   return libnewt_createGrid(list->next); */
  /*   break; */
  /* case CURSOR: */
  /*   return libnewt_cursor(list->next); */
  /*   break; */
  /* case DELAY: */
  /*   return libnewt_delay(list->next); */
  /*   break; */
  case DRAW:
    return libnewt_draw(list->next);
    break;
  /* case ENTRY: */
  /*   return libnewt_entry(list->next); */
  /*   break; */
  case FINISHED:
    return libnewt_finished(list->next);
    break;
  /* case FORM: */
  /*   return libnewt_form(list->next); */
  /*   break; */
  /* case GETSCREENSIZE: */
  /*   return libnewt_getScreenSize(list->next); */
  /*   break; */
  /* case GRID: */
  /*   return libnewt_grid(list->next); */
  /*   break; */
  case INIT:
    return libnewt_init(list->next);
    break;
  /* case LABEL: */
  /*   return libnewt_label(list->next); */
  /*   break; */
  /* case LISTBOX: */
  /*   return libnewt_listbox(list->next); */
  /*   break; */
  /* case LISTITEM: */
  /*   return libnewt_listitem(list->next); */
  /*   break; */
  case POP:
    return libnewt_pop(list->next);
    break;
  case PUSHHELPLINE:
    return libnewt_pushHelpLine(list->next);
    break;
  /* case RADIO: */
  /*   return libnewt_radio(list->next); */
  /*   break; */
  /* case REDRAWHELPLINE: */
  /*   return libnewt_redrawHelpLine(list->next); */
  /*   break; */
  case REFRESH:
    return libnewt_refresh(list->next);
    break;
  /* case RESIZESCREEN: */
  /*   return libnewt_resizeScreen(list->next); */
  /*   break; */
  /* case RESUME: */
  /*   return libnewt_resume(list->next); */
  /*   break; */
  /* case RUNFORM: */
  /*   return libnewt_runForm(list->next); */
  /*   break; */
  /* case SCALE: */
  /*   return libnewt_scale(list->next); */
  /*   break; */
  /* case SCROLLBAR: */
  /*   return libnewt_scrollbar(list->next); */
  /*   break; */
  /* case SET: */
  /*   return libnewt_set(list->next); */
  /*   break; */
  /* case SUSPEND: */
  /*   return libnewt_suspend(list->next); */
  /*   break; */
  /* case TEXTBOX: */
  /*   return libnewt_textbox(list->next); */
  /*   break; */
  /* case WAITFORKEY: */
  /*   return libnewt_waitForKey(list->next); */
  /*   break; */
  /* case WINMESSAGE: */
  /*   return libnewt_winMessage(list->next); */
  /*   break; */
    default:
      return 127;
  } 
}

int next(WORD_LIST ** plist) {
  if ((*plist)->next != NULL){
    *plist=((*plist)->next);
    return 1;
  } else {
    return 0;
  }
}

// just a macros to advance conditionnally in the list
#define NEXT(L, M)           \
  do {                       \
  if (!next(&L)) {           \
      fprintf(stderr, M);    \
      return 127;            \
 }                           \
} while(0) 

int libnewt_cls(WORD_LIST * list) {
  newtCls();
  return 0;
}

static const char * DRAW_USAGE =                \
  "argument missing\n"				\
  "usage:\n"					\
  "\tnewt draw roottext col row text\n"		\
  "\tnewt draw form component\n";

int libnewt_draw(WORD_LIST * list) {
  if (list == NULL) {
    fprintf(stderr, DRAW_USAGE);
    return 127;
  }
  if (strncmp(list->word->word,"roottext",8)==0) {
    NEXT(list, DRAW_USAGE);
    //TODO helper function to convert to int?
    int col = (int) strtol(list->word->word, NULL, 10);    
    NEXT(list, DRAW_USAGE);
    int row = (int) strtol(list->word->word, NULL, 10);
    NEXT(list, DRAW_USAGE);
    newtDrawRootText(col, row, list->word->word);
    return 0;
  } else {
    fprintf(stderr, "invalid argument for draw: %s\n", list->word->word);
    return 127;
  }
  return 0;
}



int libnewt_finished(WORD_LIST * list) {
  return newtFinished();
}

int libnewt_init(WORD_LIST * list) {
  return newtInit();
}

int libnewt_pop(WORD_LIST * list) {

}

int libnewt_pushHelpLine(WORD_LIST * list) {
  if (list != NULL) {
    newtPushHelpLine(list->word->word);
  } else {
    newtPushHelpLine(NULL);
  }
  return 0;
}

int libnewt_refresh(WORD_LIST * list) {
  newtRefresh();
  return 0;
}
