Some notes on bash sources that may help when writing a loadable builtin 

WORD_LIST
---------

The arguments of the builtin are passed as a linked list. 
The WORD_LIST is defined in command.h. 
```
/* A structure which represents a word. */
typedef struct word_desc {
  char *word;		/* Zero terminated string. */
  int flags;		/* Flags associated with this word. */
} WORD_DESC;

/* A linked list of words. */
typedef struct word_list {
  struct word_list *next;
  WORD_DESC *word;
} WORD_LIST;
```

Memory function
---------------
xmalloc.h
Defines the X memory functions, allows to use the same function as bash.
There are several wrappers depeding on how bash is compiled

with xmalloc failing to allocate the memory is fatal

General
---------------
```
/* general.h -- defines that everybody likes to use. */
```

savestring: a simple allocate + copy the string

