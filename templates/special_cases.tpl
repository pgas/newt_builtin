{# special cases this file must exists even if empty #}

{%- import 'default_macros.tpl' as macros %}

{%- macro newtComponentAddDestroyCallback(func) %}

   {{ macros.localvar('bash_newt_component', 'co') }}
   {{ macros.localvar('char *', 'callback') }}

   bash_newt_set_destroy_callback(co, callback);

   return 0;
   
   usage:
     fprintf(stderr, "newt: usage: %s %s\n", "newt ComponentAddDestroyCallback", "co callback");

   return 1;
{%- endmacro %}


{%- macro newtGetScreenSize(func) -%}

  {{ macros.localrefvar(*func.args[0]) }}
  {{ macros.localrefvar(*func.args[1]) }}
  
  {{ func.name }}({{ macros.arglist(func.args) }});

  {{ macros.bindvar(*func.args[0]) }}
  {{ macros.bindvar(*func.args[1]) }}
  
  {# {
   char *value;
   int___ptr___to_string(cols, &value);
   bash_builtin_utils_bind_variable(cols_varname, value, 0);
   xfree(value);
  }
  {
   char *value;
   int___ptr___to_string(rows, &value);
   bash_builtin_utils_bind_variable(rows_varname, value, 0);
   xfree(value);
  }
#}
  return 0;

  usage:
  fprintf(stderr, "newt: usage: %s %s\n", "{{ func.name | replace('newt','newt ') }}", "{%- for (type, name) in func.args %}{{ name }}{%- if loop.nextitem is defined %} {%endif %}{%- endfor %}"); 
  return 1;
  
{%- endmacro %}

