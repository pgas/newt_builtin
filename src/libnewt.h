#ifndef __LIBNEWT_H_
#define __LIBNEWT_H_


#include "bash_includes.h"


/* 
  parse the commands and execute accordingly
  returns 127 in case of unknown command,
  
*/ 
  
int libnewt_run(WORD_LIST * list);



#endif
