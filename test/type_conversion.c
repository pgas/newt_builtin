#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>


#include "type_conversion.h"

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

static int setup(void **state) {
  return 0;
}
static int teardown(void **state) {
  return 0;
}

void test_string_to_int_success(void **state) {
  int i;
  assert_int_equal(true, string_to_int("12", &i));
  assert_int_equal(12, i);
}


int main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_string_to_int_success, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
