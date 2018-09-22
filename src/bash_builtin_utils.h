#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdbool.h>

#include "bash_includes.h"

/*
 * used to store the pointers in a tree with tsearch
 */
int bash_builtin_utils_compare_ptr (const void *a, const void *b);

/*
 * create/update a variable "name" and store "value"
 * borrowed from printf.def 
*/
SHELL_VAR * bash_builtin_utils_bind_variable(char *name, char *value, int flags);

/*
 * true is name is a legal variable name
 */
bool bash_builtin_utils_is_a_legal_name(const char* name);

/* helpers to generate word lists */

/*
 * allocate a new word
 */
WORD_LIST * bash_builtin_utils_word(const char* string);

/*
 * allocate free a word
 */
void  bash_builtin_utils_free_word(WORD_LIST * arg);

/*
 * allocate a list of words, the arguments list must end with NULL
 */
WORD_LIST * bash_builtin_utils_make_word_list(const char* arg0, ...);
#endif
