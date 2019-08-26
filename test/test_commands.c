#include "../src/commands.h"
#include "test.h"
#include <stdlib.h>

static void test_v_command_valid(void** state) {
  (void)state;

  char* value1 = strdup("v " __FILE__);
  assert_true(command_valid(value1));
  free(value1);

  char* value2 = strdup("v " __FILE__ ":123");
  assert_true(command_valid(value2));
  free(value2);

  char* value3 = strdup("v \"" __FILE__ ":123\"");
  assert_true(command_valid(value3));
  free(value3);

  char* value4 = strdup("v '" __FILE__ ":123'");
  assert_true(command_valid(value4));
  free(value4);
}

int main()  {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_v_command_valid),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
