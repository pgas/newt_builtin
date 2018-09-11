#ifndef __BASH_NEWT_H_
#define __BASH_NEWT_H_
{#  jinja2 template for the header  #}

#include "bash_includes.h"
#include "dispatch.h"

int newt_builtin(WORD_LIST * list);

extern char *newt_doc[];
extern struct builtin newt_struct;

/* forward declaration */
{%- for name in names %}
int bash_{{ name }}(WORD_LIST *args);
{%- endfor %}

/* dispatch table */
extern entry_point entries[];
extern size_t entries_length;

#endif
