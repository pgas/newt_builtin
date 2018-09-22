#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include <newt.h>
#include "bash_newt.h"
#include "bash_builtin_utils.h"
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


void  __wrap_newtDrawRootText(int col, int row, const char * text){
  check_expected(col);
  check_expected(row);
  check_expected_ptr(text);
}

void test_draw_root_text_arguments(void **state) {
  WORD_LIST * args = bash_builtin_utils_word("drawroottext");
  args->next =  bash_builtin_utils_word("10"); 
  args->next->next =  bash_builtin_utils_word("20"); 
  
  const char * msg = "hello, world!";
  args->next->next->next =  bash_builtin_utils_word(msg); 

  expect_value(__wrap_newtDrawRootText, col, 10);
  expect_value(__wrap_newtDrawRootText, row, 20);
  expect_string(__wrap_newtDrawRootText, text, msg);

  bash_newtDrawRootText(NULL, args);

  bash_builtin_utils_free_word(args->next->next->next);
  bash_builtin_utils_free_word(args->next->next);
  bash_builtin_utils_free_word(args->next);
  bash_builtin_utils_free_word(args);

}

newtComponent __wrap_newtCompactButton(int left, int top, const char * text){
  return (newtComponent)mock();
}

newtComponent my() {
  return __wrap_newtCompactButton(0,0,"");
}

void test_newtCompactButton_store_components(void **state) {

  will_return(__wrap_newtCompactButton, (newtComponent)12);
  
  //  bash_newtCompactButton("button",)
  assert_ptr_equal(my(), (newtComponent)12);

}

int __wrap_main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_init_is_called, setup, teardown),
   cmocka_unit_test_setup_teardown(test_draw_root_text_arguments, setup, teardown),
   cmocka_unit_test_setup_teardown(test_newtCompactButton_store_components, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
