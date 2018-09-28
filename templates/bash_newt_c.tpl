{#-  jinja2 template for the implementation  #}

{%- import 'default_macros.tpl' as macros %}
{%- import 'special_cases.tpl' as special %}

#include <stdio.h>
#include "bash_newt.h"
#include "bash_builtin_utils.h"
#include "type_conversion.h"

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
int bash_{{ func.name }}( char* varname, WORD_LIST *args) {
 {%- if special[func.name] is defined %}
  {{ special[func.name](func) }}
 {%- else %}
  {%- for (type, name) in func.args %}
  {{ macros.localvar(type, name) }}
  {%- endfor %}	   
  {{ macros.return_var(func.return_type) }} {{ func.name }}({{ macros.arglist(func.args) }});

  {%- if func.return_type != "void" %}
  if (varname != NULL) {
    char * value;  
    {{ func.return_type | type_to_string }}(return_value, &value);
    bash_builtin_utils_bind_variable(varname, value, 0);
    xfree(value);
  }
  {%- endif %}

  return 0;

  usage:
  fprintf(stderr, "newt: usage: %s %s\n", "{{ func.name | replace('newt','newt ') }}", "{%- for (type, name) in func.args %}{{ name }}{%- if loop.nextitem is defined %} {%endif %}{%- endfor %}"); 
  return 1;

 {% endif %}
}
{% endfor %}
