#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "type_conversion.h"

static int setup(void **state) {
  return 0;
}
static int teardown(void **state) {
  return 0;
}

void test_string_to_newtComponent_fails(void **state) {
  newtComponent c;
  assert_false(string_to_newtComponent("12", &c));
}


void test_newtComponent_to_string(void **state) {
  newtComponent c = xmalloc(sizeof(newtComponent));
  char s[pointer_string_size];
  s[0] = 0;
  assert_true(newtComponent_to_string(c, s));
  assert_int_not_equal(0, strlen(s));
  xfree(c);
}

void test_string_to_int_success(void **state) {
  int i;
  assert_int_equal(true, string_to_int("12", &i));
  assert_int_equal(12, i);
}


int __wrap_main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_string_to_int_success, setup, teardown),
   cmocka_unit_test_setup_teardown(test_newtComponent_to_string, setup, teardown),
   cmocka_unit_test_setup_teardown(test_string_to_newtComponent_fails, setup, teardown)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
