#pragma once

#include <errno.h>
#include <stdbool.h>

int parse_port_number(const char* str);

bool str_isnumber(const char* str);

void split_filename_and_number(char* str, char** number);

void _log(const char* level, const char* fmt, ...)
  __attribute__((format(printf, 2, 3)));

#define log_info(...) _log("info", __VA_ARGS__)
#define log_error(...) _log("error", __VA_ARGS__)
#define log_perror(fmt, ...) \
  _log("error", "%s - " fmt, strerror(errno), ## __VA_ARGS__)
#define log_pfatal(fmt, ...) \
  _log("fatal", "%s - " fmt, strerror(errno), ## __VA_ARGS__)
