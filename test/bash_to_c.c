#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>


#include "bash_to_c.h"

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
static WORD_LIST *make_list(const char * word){
  WORD_LIST *root = (WORD_LIST *)xmalloc(sizeof(WORD_LIST));
  WORD_LIST * child = (WORD_LIST *)xmalloc(sizeof(WORD_LIST));
  root->next = child;
  child->word = (WORD_DESC *)xmalloc(sizeof(WORD_DESC));
  child->word->word = savestring(word);
  return root;
}

static void free_list(WORD_LIST * list){
  xfree(list->next->word->word);
  xfree(list->next->word);
  xfree(list->next);
  xfree(list);
}

static int setup(void **state) {
  return 0;
}
static int teardown(void **state) {
  return 0;
}

void test_to_int_success(void **state) {
  WORD_LIST * twelve =  make_list("12");
  int i = 0;
  assert_int_equal(0, to_int(twelve, &i));
  assert_int_equal(12, i);
  free_list(twelve);
}


int main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_to_int_success, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
