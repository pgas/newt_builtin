#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdbool.h>

#include "dispatch.h"



static int setup(void **state) {
  return 0;
}
static int teardown(void **state) {
  return 0;
}


void test_a(void **state) {
  assert_true(true);
}

int __wrap_main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_a, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
