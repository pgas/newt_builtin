#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>


#include "bash_newt.h"
/* /\* a structure which represents a word. *\/ */
/* typedef struct word_desc { */
/*   char *word;		/\* Zero terminated string. *\/ */
/*   int flags;		/\* Flags associated with this word. *\/ */
/* } WORD_DESC; */

/* /\* A linked list of words. *\/ */
/* typedef struct word_list { */
/*   struct word_list *next; */
/*   WORD_DESC *word; */
/* } WORD_LIST; */


/** make a WORD list with word as root->next **/

void  __wrap_newtInit(){
  function_called();
}

static int setup(void **state) {
  return 0;
}
static int teardown(void **state) {
  return 0;
}

void test_init_is_called(void **state) {
  expect_function_call(__wrap_newtInit); 
  bash_newtInit(NULL); 

}

int __wrap_main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_init_is_called, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
