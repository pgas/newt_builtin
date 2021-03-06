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

void __wrap_newtComponentAddDestroyCallback(newtComponent co,
				     newtCallback f, void * data){
  fprintf(stderr, "called\n");
}



void test_string_to_newtComponent_after_to_string(void **state) {
  newtComponent c1 = xmalloc(sizeof(newtComponent));
  char *s;
  assert_true(newtComponent_to_string(c1, &s));
  newtComponent c2;
  assert_true(string_to_newtComponent(s, &c2));
  assert_ptr_equal(c1,c2);
  xfree(s);
  xfree(c2);
}

void test_string_to_newtComponent_fails(void **state) {
  newtComponent c;
  assert_false(string_to_newtComponent("12", &c));
}

void test_string_to_newtComponents_when_string_is_NULL(void **state) {
  newtComponent c;
  assert_true(string_to_newtComponent("NULL", &c));
  assert_ptr_equal(NULL, c);
}



void test_newtComponent_to_string(void **state) {
  newtComponent c = xmalloc(sizeof(newtComponent));
  char *s;
  assert_true(newtComponent_to_string(c, &s));
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
   cmocka_unit_test_setup_teardown(test_string_to_newtComponent_fails, setup, teardown),
   cmocka_unit_test_setup_teardown(test_string_to_newtComponent_after_to_string,setup, teardown),
   cmocka_unit_test_setup_teardown(test_string_to_newtComponents_when_string_is_NULL,setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
