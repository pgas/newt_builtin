{# special cases this file must exists even if empty #}

{%- import 'default_macros.tpl' as macros %}

{# newtComponent newtCheckbox(int left, int top, const char * text, char defValue,
			   const char * seq, char * result); #}

{%- macro newtCheckbox(func) -%}
  {{ macros.localvar(*func.args[0]) }}
  {{ macros.localvar(*func.args[1]) }}
  {{ macros.localvar(*func.args[2]) }}
  char defValue = 0;
  char * seq = NULL;
  if ( args->next != NULL)  { 
    args = args->next;
    if (! string_to_char(args->word->word, &defValue)) {    
       goto usage;
    }
    if ( args->next != NULL) {
        args = args->next;
    	if (! string_to_char___ptr__(args->word->word, &seq)) {    
	      goto usage;
	}
    }
  }

  if (varname == NULL) { goto usage; }

  bash_newt_component bash_co = bash_newt_new(NULL);
  newtComponent return_value = {{ func.name }}({{ macros.arglist(func.args[:-1]) }}, bash_newt_get_checkbox_result(bash_co));
  bash_newt_set_newt_component(bash_co, return_value);
  char * value;
  
  void___ptr___to_string(bash_co, &value);
  bash_builtin_utils_bind_variable(varname, value, 0);
  xfree(value);
  
  return 0;

  usage:
  fprintf(stderr, "newt: usage: %s %s\n", "{{ func.name | replace('newt','newt ') }}", "{%- for (type, name) in func.args %}{{ name }}{%- if loop.nextitem is defined %} {%endif %}{%- endfor %}"); 
  return 1;
  
{%- endmacro %}



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
  
  return 0;

  usage:
  fprintf(stderr, "newt: usage: %s %s\n", "{{ func.name | replace('newt','newt ') }}", "{%- for (type, name) in func.args %}{{ name }}{%- if loop.nextitem is defined %} {%endif %}{%- endfor %}"); 
  return 1;
  
{%- endmacro %}

