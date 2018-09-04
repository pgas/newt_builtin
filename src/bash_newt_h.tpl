#ifndef __BASH_NEWT_H_
#define __BASH_NEWT_H_
{#  jinja2 template for the header  #}

#include "bash_includes.h"

{% for name in names %}
int bash_{{ name }}(WORD_LIST *args);
{% endfor %}

#endif
