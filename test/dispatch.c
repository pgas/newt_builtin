#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>


#include "dispatch.h"

static entry_point entries[] = {
     { .name = "hello" },
     { .name = "foo" }
       
};


static int setup(void **state) {
   HASH_TABLE * dispatch_table = dispatch_table_create(2, entries);
   *state = dispatch_table; 
  return 0;
}
static int teardown(void **state) {
  HASH_TABLE * dispatch_table = *state;
  hash_dispose(dispatch_table);
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



int main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_dispatch_init, setup, teardown),
   cmocka_unit_test_setup_teardown(test_dispatch_succeeds, setup, teardown)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
