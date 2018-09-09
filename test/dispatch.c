#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>


#include "dispatch.h" 

void test_dispatch_init(void **state) {
  entry_point entries[2];
  entries[0].name = "hello";
  entries[1].name = "foo";
  HASH_TABLE * dispatch_table = dispatch_table_create(2, entries);
  assert_int_equal(2, hash_size(dispatch_table));
  hash_dispose(dispatch_table);
}

void test_dispatch_succeeds(void **state) {
  entry_point entries[1];
  entries[0].name = "hello";
  entries[1].name = "foo";
  HASH_TABLE * dispatch_table = dispatch_table_create(2, entries);
  entry_point * hello = dispatch_table_find("hello", dispatch_table);
  assert_non_null(hello);
  assert_ptr_equal(&entries[0], hello);
  hash_dispose(dispatch_table);
}



int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_dispatch_init),
    cmocka_unit_test(test_dispatch_succeeds)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
