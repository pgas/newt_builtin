#ifndef __LIBNEWT_H_
#define __LIBNEWT_H_

#include <config.h>
#include <builtins.h>
#include <shell.h>


/* 
  parse the commands and execute accordingly
  returns 127 in case of unknown command,
  
*/ 
  
int libnewt_run(WORD_LIST * list);



#endif
