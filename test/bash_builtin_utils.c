#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>


#include "bash_builtin_utils.h"


static int setup(void **state) {
  return 0;
}
static int teardown(void **state) {
  return 0;
}

void test_word(void **state) {
  WORD_LIST * w = bash_builtin_utils_word("hello");
  assert_string_equal("hello", w->word->word);
  bash_builtin_utils_free_word(w);
}

void test_make_word_list(void **state) {
  WORD_LIST * w = bash_builtin_utils_make_word_list("hello", "world!", NULL);
  assert_string_equal("hello", w->word->word);
  assert_string_equal("world!", w->next->word->word);
  assert_ptr_equal(NULL, w->next->next);
  bash_builtin_utils_free_word_list(w);
}


int __wrap_main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_word, setup, teardown),
   cmocka_unit_test_setup_teardown(test_make_word_list, setup, teardown),
   /* cmocka_unit_test_setup_teardown(test_dispatch_runs_function, setup, teardown) */
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
