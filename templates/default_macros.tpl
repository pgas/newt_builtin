{#- avoid shadowing the param of our function #}

{%- macro local(name) -%}
{%- if name == 'args' %}local_{%- endif %}{{ name }}
{%- endmacro %}

{#- generate the list of arguments for the function call #}
{%- macro arglist(args) -%}
{%- for (type, name) in args %}{{ local(name) }}{%- if loop.nextitem is defined %}, {%endif %}{%- endfor %}
{%- endmacro %}

{%- macro return_var(type) -%}
{%- if type != "void" %}{{ type }} return_value = {%- endif %}
{%- endmacro %}

{%- macro localvar(type, name) %}
  if ( args->next == NULL) goto usage;
  args = args->next;
  {{ type }} {{ local(name) }};
  if (! {{ type | string_to_type }}(args->word->word, &{{ local(name) }})) {    
     goto usage;
  }  
{%- endmacro %}
