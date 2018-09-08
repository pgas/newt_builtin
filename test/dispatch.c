#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>


void test_dispatch_succeeds() {
  assert_int_equal(1,1);
}

int main(void)
{
  const UnitTest tests[] = {
    unit_test(test_dispatch_succeeds),
    /* unit_test(test2), */
  };

  return run_tests(tests);
}
