#include "../src/lib.h"
#include "test.h"
#include <stdlib.h>

static void test_parse_port_number(void** state) {
  (void)state;
  assert_int_equal(123, parse_port_number("123"));
  assert_int_equal(1, parse_port_number("1"));
  assert_int_equal(65535, parse_port_number("65535"));
  assert_int_equal(-1, parse_port_number("65536"));
  assert_int_equal(-1, parse_port_number("0"));
  assert_int_equal(-1, parse_port_number("12a"));
  assert_int_equal(-1, parse_port_number("a"));
  assert_int_equal(-1, parse_port_number(""));
}

static void test_str_isnumber(void** state) {
  (void)state;
  assert_true(str_isnumber("123"));
  assert_true(str_isnumber("0"));
  assert_false(str_isnumber("12 3"));
  assert_false(str_isnumber("123a"));
  assert_false(str_isnumber("a"));
  assert_false(str_isnumber("-1"));
}

static void test_split_filename_and_number(void** state) {
  (void)state;
  char* number = NULL;

  char* file1 = strdup(__FILE__ ":123");
  split_filename_and_number(file1, &number);
  assert_string_equal(__FILE__, file1);
  assert_string_equal("123", number);
  free(file1);

  char* file2 = strdup(__FILE__);
  number = NULL;
  split_filename_and_number(file2, &number);
  assert_string_equal(__FILE__, file2);
  assert_null(number);
  free(file2);

  char* file3 = strdup(__FILE__ ":");
  split_filename_and_number(file3, &number);
  assert_string_equal(__FILE__, file3);
  assert_null(number);
  free(file3);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_parse_port_number),
    cmocka_unit_test(test_str_isnumber),
    cmocka_unit_test(test_split_filename_and_number),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
