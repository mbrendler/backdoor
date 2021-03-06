#define _GNU_SOURCE
#include "test.h"
#include <stdio.h>
#include <stdlib.h>

int wrapped_main(int argc, const char** argv);

void __wrap_exit(int status) {
  check_expected(status);
}

int __wrap_printf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char *string;
  vasprintf(&string, fmt, ap);
  va_end(ap);
  check_expected(string);
  return 0;
}

int server_run(const char* address, int port) {
  (void)address;
  (void)port;
  check_expected(address);
  check_expected(port);
  return 23;
}

static void test_main_help(void** state) {
  (void)state;
  const char* argv[] = { "./backdoor", "--help" };
  expect_string(server_run, address, "--help");
  expect_value(server_run, port, 44444);
  expect_value(__wrap_exit, status, 0);
  expect_string(
    __wrap_printf,
    string,
    "./backdoor [-h|--help] [BIND_ADDRESS] [PORT]\n\n"
  );
  expect_string(
    __wrap_printf,
    string,
    "  BIND_ADDRESS  --  default: 127.0.0.1\n"
  );
  expect_string(
    __wrap_printf,
    string,
    "  PORT          --  default: 44444\n"
  );
  wrapped_main(2, argv);
}

static void test_main_no_args(void** state) {
  (void)state;
  const char* argv[] = { "./backdoor" };
  expect_string(server_run, address, "127.0.0.1");
  expect_value(server_run, port, 44444);
  assert_int_equal(23, wrapped_main(1, argv));
}

static void test_main_with_address(void** state) {
  (void)state;
  const char* argv[] = { "./backdoor", "1.2.3.4" };
  expect_string(server_run, address, "1.2.3.4");
  expect_value(server_run, port, 44444);
  wrapped_main(2, argv);
}

static void test_main_with_port(void** state) {
  (void)state;
  const char* argv[] = { "./backdoor", "12345" };
  expect_string(server_run, address, "127.0.0.1");
  expect_value(server_run, port, 12345);
  wrapped_main(2, argv);
}

static void test_main_two_args(void** state) {
  (void)state;
  const char* argv[] = { "./backdoor", "4.5.6.7", "12345" };
  expect_string(server_run, address, "4.5.6.7");
  expect_value(server_run, port, 12345);
  wrapped_main(3, argv);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_main_help),
    cmocka_unit_test(test_main_no_args),
    cmocka_unit_test(test_main_with_address),
    cmocka_unit_test(test_main_with_port),
    cmocka_unit_test(test_main_two_args),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
