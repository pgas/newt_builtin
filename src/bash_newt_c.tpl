{#  jinja2 template for the implementation  #}
#include <stdio.h>
#include "bash_newt.h"

/* An array of strings forming the `long' documentation for a builtin xxx,
   which is printed by `help xxx'.  It must end with a NULL.  By convention,
   the first line is a short description. */
char *newt_doc[] = {
	"Sample builtin.",
	"",
	"this is the long doc for the sample newt builtin",
	(char *)NULL
};

/* The standard structure describing a builtin command.  bash keeps an array
   of these structures.  The flags must include BUILTIN_ENABLED so the
   builtin can be used. */
struct builtin newt_struct = {
	"newt",		/* builtin name */
	newt_builtin,		/* function implementing the builtin */
	BUILTIN_ENABLED,	/* initial flags for builtin */
	newt_doc,		/* array of long documentation strings. */
	"newt",		/* usage synopsis; becomes short_doc */
	0			/* reserved for internal use */
};

/* dispatch table */
entry_point entries[] = {
 {%- for func in funcs | without_variadic %}
  { .name = "{{ func.name | replace("newt","") }}",
    .function = bash_{{ func.name }} } {% if loop.nextitem is defined %},{%- endif %}
 {%- endfor %}
};

size_t entries_length = sizeof(entries)/sizeof(entries[0]);

/* implementation of the wrappers */
{%- for func in funcs | without_variadic %}	 
int bash_{{ func.name }}(WORD_LIST *args) {
  {%- for (type, name) in func.args %}
  {#- avoid shadowing the param of our function #}
  {%- if name == 'args' %}{%- set name = 'local_args' %}{%- endif %}
  {{ type }} {{ name }};
  {%- endfor %}
  printf("function called %s\n", "{{ func.name }}");
  return 1;
}
{%- endfor %}
