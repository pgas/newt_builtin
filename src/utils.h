#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdbool.h>

#include "bash_includes.h"

/* borrowed from printf.def */
SHELL_VAR * newt_bind_variable(char *name, char *value, int flags);
bool is_a_legal_name(const char* name);

#endif
