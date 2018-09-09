#ifndef __DISPATCH_H__
#define __DISPATCH_H__

#include "bash_includes.h" 

typedef  int (*entry_function)(WORD_LIST *);

typedef struct entry_point_struct {
  char * name;
  entry_function     function;
} entry_point;

/**
 * create the dispatch table from the array of definitions
 */
HASH_TABLE *dispatch_table_create(size_t n, entry_point table[]);


/**
 * Find and return the entry point by name
 * table is a array of entry_point terminated by an entry with a NULL name
 * return NULL if not found
 */
entry_point *dispatch_table_find(const char *name, HASH_TABLE *table);

#endif
