#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>


#include "dispatch.h"



static int hello(WORD_LIST *args){
  function_called();
}

static entry_point entries[] = {
     { .name = "hello", .function = hello },
     { .name = "foo" }
       
};


static int setup(void **state) {
   HASH_TABLE * dispatch_table = dispatch_table_create(2, entries);
   *state = dispatch_table; 
  return 0;
}
static int teardown(void **state) {
  HASH_TABLE * dispatch_table = *state;
  dispatch_table_dispose(dispatch_table);
  return 0;
}


void test_dispatch_init(void **state) {
  HASH_TABLE * dispatch_table = *state;
  assert_int_equal(2, hash_size(dispatch_table));
}

void test_dispatch_succeeds(void **state) {
  HASH_TABLE * dispatch_table = *state;
  entry_point * hello = dispatch_table_find("hello", dispatch_table);
  assert_non_null(hello);
  assert_ptr_equal(&entries[0], hello);  
}


void test_dispatch_runs_function(void **state) {  
  HASH_TABLE * dispatch_table = *state;
  expect_function_call(hello);
  dispatch_table_run("hello", NULL, dispatch_table);
}



int __wrap_main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_dispatch_init, setup, teardown),
   cmocka_unit_test_setup_teardown(test_dispatch_succeeds, setup, teardown),
   cmocka_unit_test_setup_teardown(test_dispatch_runs_function, setup, teardown)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
