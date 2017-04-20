#include "libnewt.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif
#include <stdio.h>

#include <newt.h>

#include "utils.h"

#define BELL               6385076388ull                /* newtBell */
#define BUTTON             6953367461249ull             /* newtButton */
#define CENTEREDWINDOW     2410708039941065287ull       /* newtCenteredWindow */
#define CHECKBOX           7572242387314956ull          /* newtCheckbox */
#define CLEARKEYBUFFER     13602549216018221327ull      /* newtClearKeyBuffer */
#define CLS                193488487ull                 /* newtCls */
#define COMPACTBUTTON      14551555469827780296ull      /* newtCompactButton */
#define COMPONENT          249884307956162072ull        /* newtComponent */
#define CREATEGRID         8246186021644719391ull       /* newtCreateGrid */
#define CURSOR             6953406523683ull             /* newtCursor */
#define DELAY              210709892404ull              /* newtDelay */
#define DRAW               6385162067ull                /* newtDraw */
#define ENTRY              210711411031ull              /* newtEntry */
#define FINISHED           7572371893739599ull          /* newtFinished */
#define FORM               6385231225ull                /* newtForm */
#define GETSCREENSIZE      2281812110973876128ull       /* newtGetScreenSize */
#define GRID               6385270123ull                /* newtGrid */
#define INIT               6385337657ull                /* newtInit */
#define LABEL              210719225253ull              /* newtLabel */
#define LISTBOX            229473570076266ull           /* newtListbox */
#define LISTITEM           7572627812773264ull          /* newtListitem */
#define OPENWINDOW         8246740163551776335ull       /* newtOpenWindow */
#define POP                193502740ull                 /* newtPop */
#define PUSH               6385597157ull                /* newtPush */
#define RADIO              210726343092ull              /* newtRadio */
#define REDRAWHELPLINE     15850615990170578747ull      /* newtRedrawHelpLine */
#define REFRESH            229481146857044ull           /* newtRefresh */
#define RESIZESCREEN       15717840087368190839ull      /* newtResizeScreen */
#define RESUME             6953974617878ull             /* newtResume */
#define RUNFORM            229481782090318ull           /* newtRunForm */
#define SCALE              210727597709ull              /* newtScale */
#define SCROLLBAR          249906305487309321ull        /* newtScrollbar */
#define SET                193505681ull                 /* newtSet */
#define SUSPEND            229483079836231ull           /* newtSuspend */
#define TEXTBOX            229483751208051ull           /* newtTextbox */
#define WAITFORKEY         8247090536862262954ull       /* newtWaitForKey */
#define WINMESSAGE         8247101992148781368ull       /* newtWinMessage */





/* forward declarations */
int libnewt_bell(WORD_LIST *);
int libnewt_centeredWindow(WORD_LIST *);
int libnewt_clearKeyBuffer(WORD_LIST *);
int libnewt_cls(WORD_LIST *);
int libnewt_draw(WORD_LIST *);
int libnewt_finished(WORD_LIST *);
extern int libnewt_form(WORD_LIST *);
int libnewt_getScreenSize(WORD_LIST *);
int libnewt_init(WORD_LIST *);
int libnewt_OpenWindow(WORD_LIST *);
int libnewt_pop(WORD_LIST *);
int libnewt_push(WORD_LIST *);
int libnewt_refresh(WORD_LIST *);
int libnewt_waitForKey(WORD_LIST *);


/* 
  parse the commands and execute accordingly
  returns 127 in case of unknown command,
  
*/ 
  
int libnewt_run(WORD_LIST * list) {
  /*   lowercase the word */
  lower(&list->word->word);

  switch (djb2_hash(list->word->word)) {
  case BELL:
    return libnewt_bell(list->next);
    break;
  /* case BUTTON: */
  /*   return libnewt_button(list->next); */
  /*   break; */
  case CENTEREDWINDOW:
    return libnewt_centeredWindow(list->next);
    break;
  /* case CHECKBOX: */
  /*   return libnewt_checkbox(list->next); */
  /*   break; */
  case CLEARKEYBUFFER:
    return libnewt_clearKeyBuffer(list->next);
    break;
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
  case FORM:
    return libnewt_form(list->next);
    break;
  case GETSCREENSIZE:
    return libnewt_getScreenSize(list->next);
    break;
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
  case OPENWINDOW:
    return libnewt_OpenWindow(list->next);
    break;
  case POP:
    return libnewt_pop(list->next);
    break;
  case PUSH:
    return libnewt_push(list->next);
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
  case WAITFORKEY:
    return libnewt_waitForKey(list->next);
    break;
  /* case WINMESSAGE: */
  /*   return libnewt_winMessage(list->next); */
  /*   break; */
    default:
      return 127;
  } 
}


int libnewt_bell(WORD_LIST *list){
  newtBell();
  return 0;
}

static const char * CENTEREDWINDOW_USAGE =      \
  "argument missing\n"				\
  "usage:\n"					\
  "\tnewt centeredWindow width height [title]\n";	

int libnewt_centeredWindow(WORD_LIST * list) {
  int width, height;

  NOT_NULL(list, CENTEREDWINDOW_USAGE);
  width = (int) strtol(list->word->word, NULL, 10);    
  NEXT(list, CENTEREDWINDOW_USAGE);
  height = (int) strtol(list->word->word, NULL, 10);

  if (list->next == NULL) {
    return newtCenteredWindow(width, height, NULL);
  } else {
    return newtCenteredWindow(width, height, list->next->word->word);
  }

}

int libnewt_clearKeyBuffer(WORD_LIST *list){
  newtClearKeyBuffer();
  return 0;
}

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
  NOT_NULL(list, DRAW_USAGE);
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
    fprintf(stderr, "%s",  DRAW_USAGE);
    return 127;
  }
  return 0;
}



int libnewt_finished(WORD_LIST * list) {
  return newtFinished();
}


int libnewt_getScreenSize(WORD_LIST *list) {
  int cols;
  int rows;
  char scols[15];
  char srows[15];
  newtGetScreenSize(&cols, &rows);
  sprintf(scols,"%d",cols);
  bind_variable("NEWT_COLS", scols, 0);
  sprintf(srows,"%d",rows);
  bind_variable("NEWT_ROWS", srows, 0);
  return 0;
}

int libnewt_init(WORD_LIST * list) {
  return newtInit();
}

static const char * OPENWINDOW_USAGE =      \
  "argument missing\n"				\
  "usage:\n"					\
  "\tnewt openWindow left top  width height [title]\n";	

int libnewt_OpenWindow(WORD_LIST *list) {
  int left, top, width, height;

  NOT_NULL(list, CENTEREDWINDOW_USAGE);
  left = (int) strtol(list->word->word, NULL, 10);    
  NEXT(list, CENTEREDWINDOW_USAGE);
  top = (int) strtol(list->word->word, NULL, 10);    
  NEXT(list, CENTEREDWINDOW_USAGE);
  width = (int) strtol(list->word->word, NULL, 10);    
  NEXT(list, CENTEREDWINDOW_USAGE);
  height = (int) strtol(list->word->word, NULL, 10);

  if (list->next == NULL) {
    return newtOpenWindow(left, top, width, height, NULL);
  } else {
    return newtOpenWindow(left, top, width, height, list->next->word->word);
  }
}

static const char * POP_USAGE =                \
  "argument missing\n"				\
  "usage:\n"					\
  "\tnewt pop window|windownorefresh|helpline\n";

int libnewt_pop(WORD_LIST * list) {
  NOT_NULL(list, POP_USAGE);
  if (strncmp(list->word->word,"helpline",8)==0) {
    newtPopHelpLine();
  } else if (strncmp(list->word->word,"window",6)==0) {
    newtPopWindow();
  } else {
    fprintf(stderr, "%s",  POP_USAGE);
    return 127;
  }
  return 0;
}

static const char * PUSH_USAGE =                \
  "argument missing\n"				\
  "usage:\n"					\
  "\tnewt push helpline [message]\n";

int libnewt_push(WORD_LIST * list) {
  if ((list != NULL)
      && (strncmp(list->word->word,"helpline",8)==0)) {
    if (list->next!=NULL) {
      newtPushHelpLine(list->next->word->word);
    } else {
      newtPushHelpLine(NULL);
    }
  } else {
    fprintf(stderr, "%s",  PUSH_USAGE);
    return 127;
  }
  return 0;
}

int libnewt_refresh(WORD_LIST * list) {
  newtRefresh();
  return 0;
}

int libnewt_waitForKey(WORD_LIST * list) {
  newtWaitForKey();
  return 0;
}
