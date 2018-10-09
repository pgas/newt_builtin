#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdbool.h>

#include "dispatch.h"
#include "bash_newt_types.h"


static int setup(void **state) {
  return 0;
}
static int teardown(void **state) {
  return 0;
}


void __wrap_newtComponentAddDestroyCallback(newtComponent co,
				     newtCallback f, void * data){
  
}

void test_newt_get(void **state) {
  newtComponent c = (newtComponent) 0x12;
  bash_newt_component bco = bash_newt_new(c);
  assert_ptr_equal(0x12, bash_newt_get_newt_component(bco));
  xfree(bco);
}

void test_newt_set(void **state) {
  bash_newt_component bco = bash_newt_new(NULL);
  newtComponent c = (newtComponent) 0x12;
  bash_newt_set_newt_component(bco, c);
  assert_ptr_equal(0x12, bash_newt_get_newt_component(bco));
  xfree(bco);
}



int __wrap_main(void)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test_setup_teardown(test_newt_get, setup, teardown),
    cmocka_unit_test_setup_teardown(test_newt_set, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
