#ifndef __TYPE_CONVERSION_H__
#define __TYPE_CONVERSION_H__

#include <stdbool.h>
#include <newt.h>

#include "bash_includes.h"


/* size of a string holding the pointer 
 representation.
*/
extern const size_t pointer_string_size;

/**
 * conversions from the types used by libnewt to string
 * the caller must allocate enough space.
 */

bool char___ptr___to_string(char* value, char** result);
bool char_to_string(char value, char** result);
bool int___ptr___to_string(int* value, char** result);
bool int_to_string(int value, char** result);
bool newtComponent_to_string(newtComponent value, char** result);
bool newtGrid_to_string(newtGrid value, char** result);
bool void___ptr____ptr___to_string(void ** value, char** result);
bool void___ptr___to_string(void *value, char** result);

/**
 *conversions from strings to types used by libnewt
 */

bool string_to_int(const char *, int * result);
bool string_to_newtComponent(const char* value, newtComponent *result);
bool string_to_enum_newtGridElement(const char* value, enum newtGridElement *result);
bool string_to_newtSuspendCallback(const char* value, newtSuspendCallback *result);
bool string_to_newtComponent___ptr__(const char* value, newtComponent * *result);
bool string_to_newtEntryFilter(const char* value, newtEntryFilter *result);
bool string_to_newtGrid(const char* value, newtGrid *result);
bool string_to_unsigned_int(const char* value, unsigned int *result);
bool string_to_va_list(const char* value, va_list *result);
bool string_to_int___ptr__(const char* value, int * *result);
bool string_to_void___ptr____ptr__(const char* value, void ** *result);
bool string_to_long_long(const char* value, long long *result);
bool string_to_newtCallback(const char* value, newtCallback *result);
bool string_to_struct_newtColors(const char* value, struct newtColors *result);
bool string_to_char(const char* value, char *result);
bool string_to_struct_newtExitStruct___ptr__(const char* value, struct newtExitStruct * *result);
bool string_to_void___ptr__(const char* value, void * *result);
bool string_to_enum_newtFlagsSense(const char* value, enum newtFlagsSense *result);
bool string_to_char___ptr____ptr__(const char* value, char ** *result);
bool string_to_char___ptr__(const char* value, char * * result);
bool string_to_unsigned_long_long(const char* value, unsigned long long *result);

#endif
