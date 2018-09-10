#ifndef __BASH_NEWT_H_
#define __BASH_NEWT_H_
{#  jinja2 template for the header  #}

#include "bash_includes.h"

int newt_builtin(WORD_LIST * list);

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


{% for name in names %}
int bash_{{ name }}(WORD_LIST *args);
{% endfor %}

#endif
