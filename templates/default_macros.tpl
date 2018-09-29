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

{%- macro localrefvar(type, name) %}
  if ( args->next == NULL) goto usage;
  args = args->next;
  char *  {{  local(name) }}_varname = args->word->word;
  {{ type | replace('*','') }} {{  local(name) }}[1];
{%- endmacro %}


{%- macro localvar(type, name) %}
  if ( args->next == NULL) goto usage;
  args = args->next;
  {{ type }} {{ local(name) }};
  if (! {{ type | string_to_type }}(args->word->word, &{{ local(name) }})) {    
     goto usage;
  }
{%- endmacro %}

{%- macro bindvar(type, name) %}
  {
   char *value;
   {{ type | type_to_string }}({{ local(name) }}, &value);
   bash_builtin_utils_bind_variable({{ local(name) }}_varname, value, 0);
   xfree(value);
  }
{%- endmacro %}


{%- macro basic_fun(func) -%}
  {%- for (type, name) in func.args %}
  {{ localvar(type, name) }}
  {%- endfor %}	   
  {{ return_var(func.return_type) }} {{ func.name }}({{ arglist(func.args) }});

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
  
{%- endmacro %}