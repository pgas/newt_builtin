#ifndef __BASH_NEWT_H_
#define __BASH_NEWT_H_
{#  jinja2 template for the header  #}

#include <newt.h>

#include "bash_includes.h"
#include "dispatch.h"

int newt_builtin(WORD_LIST * list);

extern char *newt_doc[];
extern struct builtin newt_struct;

/* forward declaration */
{%- for func in funcs | without_variadic %}
{#- skip variadic function for now, they are tricky to call (libffi..)
    and I suspect they are just convenience function #}	 
int bash_{{ func.name }}(WORD_LIST *args);
{%- endfor %}

/* dispatch table */
extern entry_point entries[];
extern size_t entries_length;

#endif
