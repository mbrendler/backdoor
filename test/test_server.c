#define _GNU_SOURCE
#include "test.h"
#include "../src/server.h"
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

bool command_valid(const char* buffer) {
  check_expected(buffer);
  return mock();
}

void _log(const char* level, const char* fmt, ...) {
  check_expected(level);
  check_expected(fmt);
}

int __wrap_socket(int domain, int type, int protocol) {
  const int result = mock();
  if (-1 == result) {
    check_expected(domain);
    check_expected(type);
    check_expected(protocol);
  }
  return result;
}

int __wrap_bind(
  int socket,
  const struct sockaddr *address,
  socklen_t address_len
) {
  const int result = mock();
  if (-1 == result) {
    check_expected(socket);
    check_expected(address);
    check_expected(address_len);
  }
  return result;
}

int __wrap_listen(int socket, int backlog) {
  const int result = mock();
  if (-1 == result) {
    check_expected(socket);
    check_expected(backlog);
  }
  return result;
}

int __wrap_accept(
  int socket,
  struct sockaddr *restrict address,
  const socklen_t *restrict address_len
) {
  (void)address;
  (void)address_len;
  const int result = mock();
  if (-1 == result) {
    check_expected(socket);
  }
  return result;
}

ssize_t __wrap_read(int fildes, void *buf, size_t nbyte) {
  (void)buf;
  const int result = mock();
  if (0 < result) {
    check_expected(fildes);
    check_expected(nbyte);
    for (int i = 0; i < result - 1; ++i) {
      ((char*)buf)[i] = 'a';
    }
    ((char*)buf)[result - 1] = '\0';
  }
  return result;
}

int __wrap_close(int fildes) {
  check_expected(fildes);
  return 0;
}

int __wrap_system(const char* command) {
  check_expected(command);
  return 0;
}

static void test_server_run_socket_error(void** state) {
  (void)state;
  will_return(__wrap_socket, -1);

  expect_value(__wrap_socket, domain, AF_INET);
  expect_value(__wrap_socket, type, SOCK_STREAM);
  expect_value(__wrap_socket, protocol, 0);

  expect_string(_log, level, "fatal");
  expect_string(_log, fmt, "%s - server socket");

  assert_int_equal(1, server_run("127.0.0.1", 42));
}

static void test_server_run_bind_error(void** state) {
  (void)state;
  will_return(__wrap_socket, 23);
  will_return(__wrap_bind, -1);

  struct sockaddr_in servaddr = {
    .sin_family = AF_INET,
    .sin_addr = { .s_addr = inet_addr("127.0.0.1") },
    .sin_port = htons(42)
  };

  expect_value(__wrap_bind, socket, 23);
  expect_memory(__wrap_bind, address, &servaddr, sizeof(servaddr));
  expect_value(__wrap_bind, address_len, sizeof(servaddr));

  expect_string(_log, level, "fatal");
  expect_string(_log, fmt, "%s - server bind");

  assert_int_equal(1, server_run("127.0.0.1", 42));
}

static void test_server_run_listen_error(void** state) {
  (void)state;
  will_return(__wrap_socket, 23);
  will_return(__wrap_bind, 0);
  will_return(__wrap_listen, -1);

  expect_value(__wrap_listen, socket, 23);
  expect_value(__wrap_listen, backlog, 5);

  expect_string(_log, level, "fatal");
  expect_string(_log, fmt, "%s - server listen");

  assert_int_equal(1, server_run("127.0.0.1", 42));
}

static void test_server_run_accept_error(void** state) {
  (void)state;
  will_return(__wrap_socket, 23);
  will_return(__wrap_bind, 0);
  will_return(__wrap_listen, 0);
  will_return(__wrap_accept, -1);

  expect_value(__wrap_accept, socket, 23);

  expect_string(_log, level, "fatal");
  expect_string(_log, fmt, "%s - server accept");

  assert_int_equal(1, server_run("127.0.0.1", 42));
}

static void test_server_run_read_error(void** state) {
  (void)state;
  will_return(__wrap_socket, 23);
  will_return(__wrap_bind, 0);
  will_return(__wrap_listen, 0);
  will_return(__wrap_accept, 394);
  will_return(__wrap_accept, -1);
  will_return(__wrap_read, -1);

  expect_value(__wrap_accept, socket, 23);
  expect_value(__wrap_close, fildes, 394);

  expect_string(_log, level, "error");
  expect_string(_log, fmt, "%s - server read");
  expect_string(_log, level, "fatal");
  expect_string(_log, fmt, "%s - server accept");

  assert_int_equal(1, server_run("127.0.0.1", 42));
}

static void test_server_run_command_not_valid(void** state) {
  (void)state;
  will_return(__wrap_socket, 23);
  will_return(__wrap_bind, 0);
  will_return(__wrap_listen, 0);
  will_return(__wrap_accept, 394);
  will_return(__wrap_accept, -1);
  will_return(__wrap_read, 5);
  will_return(command_valid, false);

  expect_value(__wrap_read, fildes, 394);
  expect_value(__wrap_read, nbyte, 4095);
  expect_value(__wrap_accept, socket, 23);
  expect_string(command_valid, buffer, "aaaa");
  expect_value(__wrap_close, fildes, 394);

  expect_string(_log, level, "fatal");
  expect_string(_log, fmt, "%s - server accept");

  assert_int_equal(1, server_run("127.0.0.1", 42));
}

static void test_server_run_command_valid(void** state) {
  (void)state;
  will_return(__wrap_socket, 23);
  will_return(__wrap_bind, 0);
  will_return(__wrap_listen, 0);
  will_return(__wrap_accept, 394);
  will_return(__wrap_accept, -1);
  will_return(__wrap_read, 5);
  will_return(command_valid, true);

  expect_value(__wrap_read, fildes, 394);
  expect_value(__wrap_read, nbyte, 4095);
  expect_value(__wrap_accept, socket, 23);
  expect_string(command_valid, buffer, "aaaa");
  expect_string(__wrap_system, command, "aaaa");
  expect_value(__wrap_close, fildes, 394);

  expect_string(_log, level, "info");
  expect_string(_log, fmt, "run '%s'");
  expect_string(_log, level, "fatal");
  expect_string(_log, fmt, "%s - server accept");

  assert_int_equal(1, server_run("127.0.0.1", 42));
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_server_run_socket_error),
    cmocka_unit_test(test_server_run_bind_error),
    cmocka_unit_test(test_server_run_listen_error),
    cmocka_unit_test(test_server_run_accept_error),
    cmocka_unit_test(test_server_run_read_error),
    cmocka_unit_test(test_server_run_command_not_valid),
    cmocka_unit_test(test_server_run_command_valid),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
