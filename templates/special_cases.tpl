{# special cases this file must exists even if empty #}

{%- import 'default_macros.tpl' as macros %}

{%- macro newtComponentAddDestroyCallback(func) %}

   {{ macros.localvar('newtComponent', 'co') }}
   {{ macros.localvar('char', 'callback') }}

   bash_newt_set_destroy_callback('co', 'callback');

   usage:
     fprintf(stderr, "newt: usage: %s %s\n", "newt ComponentAddDestroyCallback", "co callback"); 
{%- endmacro %}

