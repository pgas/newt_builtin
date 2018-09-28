#ifndef __BASH_NEWT_TYPE_H__
#define __BASH_NEWT_TYPE_H__
/* wrappers around newt component */

#include <newt.h>

struct bash_newt_component_struct;
typedef struct bash_newt_component_struct *  bash_newt_component;

bash_newt_component bash_newt_new(newtComponent co);

newtComponent bash_newt_get_newt_component(bash_newt_component co);

void bash_newt_set_destroy_callback(bash_newt_component co, const char * callback);

#endif
