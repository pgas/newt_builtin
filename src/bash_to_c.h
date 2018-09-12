#ifndef __BASH_TO_C_H__
#define __BASH_TO_C_H__

#include "bash_includes.h"

/**
 * try to convert args->next to int and store the conversion in result
 * return 0 if successfull
 * -1 if args->next is null
 * -2 if the conversion fails
 */

int to_int(WORD_LIST * args, int * result);



#endif
