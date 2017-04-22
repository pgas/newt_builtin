#ifndef __LIBNEWT_H_
#define __LIBNEWT_H_


#include "bash_includes.h"
#include "newt.h"


struct bash_component_struct {
  newtComponent  co;
  char * filter;
  char * callback;
};

typedef struct bash_component_struct *  bash_component;

bash_component new_bash_component(newtComponent co);

/* 
  parse the commands and execute accordingly
  returns EX_USAGE
 in case of unknown command,
  
*/ 
int libnewt_run(WORD_LIST * list);



#endif
