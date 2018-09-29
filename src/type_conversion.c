#include "type_conversion.h" 

#include <stdio.h>

#include "bash_builtin_utils.h"
#include "bash_newt_types.h"

/* TODO: configure could perhaps figure out  the value
and store it in a configure.h
 */
const size_t pointer_string_size = 30;



bool char___ptr___to_string(char* value, char** result){
  return false;
}

bool char_to_string(char value, char** result){
  return false;
}

bool int___ptr___to_string(int* value, char** result){
  *result = xmalloc(pointer_string_size);
  return snprintf(*result, pointer_string_size, "%d", *value) > 0;
}

bool int_to_string(int value, char** result){
  return false;
}

bool newtComponent_to_string(newtComponent value, char** result){
  *result = xmalloc(pointer_string_size);
  bash_newt_component comp = bash_newt_new(value);
  return snprintf(*result, pointer_string_size, "%p", comp) > 0;
  return false;
}

bool newtGrid_to_string(newtGrid value, char** result){
  return false;
}

bool void___ptr____ptr___to_string(void ** value, char** result){
  return false;
}

bool void___ptr___to_string(void *value, char** result){
  return false;
}


/*
 * silently truncate
 */

bool string_to_int(const char * value, int * result){
  intmax_t i;
  if (legal_number (value, &i)) {
    *result = (int) i;
    return true;
  }
  return false;
}


bool string_to_bash_newt_component(const char* value, bash_newt_component *result){
  if (strncmp(value, "NULL", 4) == 0) {
    *result = NULL;
    return true;
  }
  bash_newt_component  bash_co;
  if (sscanf(value, "%p", &bash_co) == 1) {
    *result  = bash_co;
    return (*result != NULL);
  } else {
    return false;
  }
}


/* only allow pointers that have previously been exported*/
bool string_to_newtComponent(const char* value, newtComponent *result){
  if (strncmp(value, "NULL", 4) == 0) {
    *result = NULL;
    return true;
  }
  bash_newt_component  bash_co;
  if (sscanf(value, "%p", &bash_co) == 1) {
    *result  = bash_newt_get_newt_component(bash_co);
    return (*result != NULL);
  } else {
    return false;
  }
}

bool string_to_enum_newtGridElement(const char* value, enum newtGridElement *result){
    return false;
}

bool string_to_newtSuspendCallback(const char* value, newtSuspendCallback *result){
    return false;
}

bool string_to_newtComponent___ptr__(const char* value, newtComponent * *result){
    return false;
}

bool string_to_newtEntryFilter(const char* value, newtEntryFilter *result){
    return false;
}

bool string_to_newtGrid(const char* value, newtGrid *result){
    return false;
}

bool string_to_unsigned_int(const char* value, unsigned int *result){
  intmax_t i;
  if (legal_number (value, &i)) {
    *result = (unsigned int) i;
    return true;
  }
  return false;
}

bool string_to_va_list(const char* value, va_list *result){
    return false;
}

bool string_to_int___ptr__(const char* value, int * *result){
    return false;
}

bool string_to_void___ptr____ptr__(const char* value, void ** *result){
    return false;
}

bool string_to_long_long(const char* value, long long *result){
    return false;
}

bool string_to_newtCallback(const char* value, newtCallback *result){
    return false;
}

bool string_to_struct_newtColors(const char* value, struct newtColors *result){
    return false;
}

bool string_to_char(const char* value, char *result){
    return false;
}

bool string_to_struct_newtExitStruct___ptr__(const char* value, struct newtExitStruct * *result){
    return false;
}

bool string_to_void___ptr__(const char* value, void * *result){
  /* for help tag in Form...it's a string */
  *result = (void *)value;
  return true;
}

bool string_to_enum_newtFlagsSense(const char* value, enum newtFlagsSense *result){
    return false;
}

bool string_to_char___ptr____ptr__(const char* value, char ** *result){
    return false;
}

bool string_to_char___ptr__(const char* value, char **  result){
  *result = value;
  return true;
}

/*
 * probably not ok for large values
 */
bool string_to_unsigned_long_long(const char* value, unsigned long long *result){
  intmax_t i;
  if (legal_number (value, &i)) {
    *result = (unsigned long long) i;
    return true;
  }
  return false;
}

  
