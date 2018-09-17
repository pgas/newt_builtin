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
  bash_newtInit(NULL, NULL); 
}

WORD_LIST * word(const char* string) {
  WORD_LIST * args = xmalloc(sizeof(WORD_LIST));
  args->word =  xmalloc(sizeof(WORD_DESC));
  args->word->word = savestring(string);
  return args;

}

void  free_word(WORD_LIST * args) {
  xfree(args->word->word);
  xfree(args->word);
  xfree(args);
}

void  __wrap_newtDrawRootText(int col, int row, const char * text){
  check_expected(col);
  check_expected(row);
  check_expected_ptr(text);
}

void test_draw_root_text_arguments(void **state) {
  WORD_LIST * args = word("drawroottext");
  args->next = word("10"); 
  args->next->next = word("20"); 
  
  const char * msg = "hello, world!";
  args->next->next->next = word(msg); 
  printf(" before calling?\n" );
  fflush(stdout);
  expect_value(__wrap_newtDrawRootText, col, 10);
  expect_value(__wrap_newtDrawRootText, row, 20);
  expect_string(__wrap_newtDrawRootText, text, msg);

  bash_newtDrawRootText(NULL, args);

  free_word(args->next->next->next);
  free_word(args->next->next);
  free_word(args->next);
  free_word(args);

}


int __wrap_main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_init_is_called, setup, teardown),
   cmocka_unit_test_setup_teardown(test_draw_root_text_arguments, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
